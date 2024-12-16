from flask import Flask, request, jsonify
from database_client import DatabaseClient
import random
import json
import time

app = Flask(__name__)
db_client = DatabaseClient()  # Настройте host и port, если требуется

def generate_user_key():
    return str(random.randint(1000000000, 9999999999))

def get_balance(user_id, lot_id):
    """Получает текущий баланс пользователя для конкретного лота."""
    bal = db_client.select("number, quantity", "user_lot", f"user_id = {user_id} AND lot_id = {lot_id}")
    if bal:
        return bal[0]["number"], float(bal[0]["quantity"])
    else:
        return None, 0.0

def update_balance(user_id, lot_id, new_quantity):
    """Обновляет баланс пользователя: удаляем старую запись, вставляем новую."""
    num, q = get_balance(user_id, lot_id)
    if num is not None:
        # Удаляем старую запись
        db_client.delete("user_lot", f"number = '{num}'")
    # Если количество 0, можно вообще не вставлять запись, но для простоты вставим
    if new_quantity > 0:
        db_client.insert("user_lot", f"{user_id}, {lot_id}, {new_quantity}")

def partial_execute_order(buy_order, sell_order, executed_qty, match_price):
    """
    Частично исполняет ордера:
    buy_order и sell_order - это словари с полями ордера.
    executed_qty - исполняемое количество базового актива.
    match_price - цена сделки.
    Возвращает (buy_order_new_qty, sell_order_new_qty, total_spent_buy, total_received_sell)
    """
    # Предполагается, что при создании buy-ордера покупатель зарезервировал quote-лот
    # При создании sell-ордера продавец зарезервировал base-лот
    # Пара: base_lot_id, quote_lot_id
    pair = db_client.select("*", "pair", f"pair_id = {buy_order['pair_id']}")
    base_lot_id = pair[0]["first_lot_id"]
    quote_lot_id = pair[0]["second_lot_id"]

    # Покупатель (buy) получает executed_qty base-лота, отдает executed_qty * match_price quote-лота
    # Продавец (sell) получает executed_qty * match_price quote-лота, отдает executed_qty base-лота

    # Обновляем балансы:
    # Покупатель:
    # Добавить base-лот:
    buyer_num, buyer_bal_base = get_balance(buy_order["user_id"], base_lot_id)
    new_buyer_bal_base = buyer_bal_base + executed_qty
    update_balance(buy_order["user_id"], base_lot_id, new_buyer_bal_base)

    # У покупателя были зарезервированы средства в quote-лоте, он их уже "списал" при создании ордера,
    # фактически баланс quote у него уже уменьшен. Так что дополнительно ничего списывать не нужно.
    # Но для корректного подсчёта refund в конце матчинга мы будем считать total_spent.

    total_spent_buy = executed_qty * match_price

    # Продавец:
    # Продавец отдаёт base-лот (он был зарезервирован при создании SELL ордера)
    # Нужно уменьшить его баланс base-лота на executed_qty
    sell_num, seller_bal_base = get_balance(sell_order["user_id"], base_lot_id)
    # Он уже должен был зарезервировать base-лот при создании ордера.
    new_seller_bal_base = max(seller_bal_base - executed_qty, 0)
    update_balance(sell_order["user_id"], base_lot_id, new_seller_bal_base)

    # Продавец получает quote-лот (добавляем executed_qty * match_price)
    sell_num_q, seller_bal_quote = get_balance(sell_order["user_id"], quote_lot_id)
    new_seller_bal_quote = seller_bal_quote + (executed_qty * match_price)
    update_balance(sell_order["user_id"], quote_lot_id, new_seller_bal_quote)

    total_received_sell = executed_qty * match_price

    # Обновляем ордера (уменьшаем количество)
    new_buy_qty = buy_order["quantity"] - executed_qty
    new_sell_qty = sell_order["quantity"] - executed_qty

    return new_buy_qty, new_sell_qty, total_spent_buy, total_received_sell

def recreate_order(order):
    """
    Пересоздаёт ордер в БД по данным словаря order.
    Формат: { "order_id": ..., "user_id": ..., "pair_id": ..., "quantity": ..., "price": ..., "type": ..., "closed": ... }
    Перед вставкой удаляем старую запись.
    """
    db_client.delete("order", f"order_id = {order['order_id']}")
    db_client.insert("order", f"{order['user_id']}, {order['pair_id']}, {order['quantity']}, {order['price']}, '{order['type']}', {order['closed']}")

@app.route('/')
def home():
    return "Crypto Exchange API is running!"

@app.route('/select', methods=['GET'])
def select_data():
    columns = request.args.get('columns')
    table = request.args.get('table')
    condition = request.args.get('condition')

    if not columns or not table:
        return jsonify({"error": "Missing required parameters: 'columns' and 'table'"}), 400

    try:
        response = db_client.select(columns, table, condition)
        return jsonify({"status": "success", "data": response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/user', methods=['POST'])
def create_user():
    data = request.json
    username = data.get('username')
    if not username:
        return jsonify({"error": "Отсутствует параметр 'username'"}), 400

    user_key = generate_user_key()
    try:
        user_response = db_client.insert("user", f"'{username}', '{user_key}'")
        user = db_client.select("*", "user", f"key = '{user_key}'")
        if not user:
            raise RuntimeError("Failed to retrieve user_id after insertion.")
        user_id = user[0]["user_id"]

        # Инициализация баланса
        lots = db_client.select("*", "lot")
        for lot in lots:
            db_client.insert("user_lot", f"{user_id}, {lot['lot_id']}, 1000")

        return jsonify({"status": "Пользователь создан", "key": user_key, "db_response": user_response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/pairs', methods=['GET'])
def get_pairs():
    try:
        response = db_client.select("*", "pair")
        return jsonify({"status": "success", "data": response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/lots', methods=['GET'])
def get_lots():
    try:
        response = db_client.select("*", "lot")
        return jsonify({"status": "success", "data": response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/order', methods=['POST'])
def create_order():
    """Создание нового ордера с проверкой и обновлением баланса."""
    data = request.json
    user_key = request.headers.get('X-USER-KEY')
    pair_id = data.get('pair_id')
    quantity = float(data.get('quantity'))
    price = float(data.get('price'))
    order_type = data.get('type')  # 'buy' или 'sell'

    if not all([user_key, pair_id, quantity, price, order_type]):
        return jsonify({"error": "Missing required fields"}), 400

    try:
        # Получаем user_id по ключу пользователя
        user = db_client.select("user_id", "user", f"key = '{user_key}'")
        if not user:
            return jsonify({"error": "User not found"}), 404
        user_id = user[0]["user_id"]

        # Получаем информацию о валютной паре
        pair = db_client.select("*", "pair", f"pair_id = {pair_id}")
        if not pair:
            return jsonify({"error": "Pair not found"}), 404

        # Определяем лоты для продажи и покупки
        lot_to_sell = pair[0]["second_lot_id"] if order_type == "buy" else pair[0]["first_lot_id"]
        lot_to_buy = pair[0]["first_lot_id"] if order_type == "buy" else pair[0]["second_lot_id"]

        # Рассчитываем необходимое количество валюты для покупки или продажи
        required_amount = quantity * price if order_type == "buy" else quantity

        # Получаем текущий баланс пользователя для валюты продажи
        user_sell_balance = db_client.select("number, quantity", "user_lot", f"user_id = {user_id} AND lot_id = {lot_to_sell}")
        if not user_sell_balance or float(user_sell_balance[0]["quantity"]) < required_amount:
            return jsonify({"error": "Insufficient balance"}), 400

        # Поиск обратных ордеров
        reverse_order_type = 'sell' if order_type == 'buy' else 'buy'
        existing_orders = db_client.select("*", "order", f"pair_id = {pair_id} AND type = '{reverse_order_type}' AND closed = '0'")

        remaining_quantity = quantity

        for existing_order in existing_orders:
            match_quantity = min(float(existing_order["quantity"]), remaining_quantity)

            # Покупатель и продавец
            buyer_id = user_id if order_type == 'buy' else existing_order["user_id"]
            seller_id = existing_order["user_id"] if order_type == 'buy' else user_id

            # Уменьшаем баланс у покупателя (валюта продажи)
            buyer_sell_balance = db_client.select("number, quantity", "user_lot", f"user_id = {buyer_id} AND lot_id = {lot_to_sell}")
            new_buyer_sell_balance = float(buyer_sell_balance[0]["quantity"]) - (match_quantity * price)
            db_client.delete("user_lot", f"number = '{buyer_sell_balance[0]['number']}'")
            db_client.insert("user_lot", f"{buyer_id}, {lot_to_sell}, {new_buyer_sell_balance}")

            # Увеличиваем баланс у покупателя (покупаемый актив)
            buyer_buy_balance = db_client.select("number, quantity", "user_lot", f"user_id = {buyer_id} AND lot_id = {lot_to_buy}")
            new_buyer_buy_balance = float(buyer_buy_balance[0]["quantity"]) + match_quantity
            db_client.delete("user_lot", f"number = '{buyer_buy_balance[0]['number']}'")
            db_client.insert("user_lot", f"{buyer_id}, {lot_to_buy}, {new_buyer_buy_balance}")

            # Уменьшаем баланс у продавца (продаваемый актив)
            seller_sell_balance = db_client.select("number, quantity", "user_lot", f"user_id = {seller_id} AND lot_id = {lot_to_buy}")
            new_seller_sell_balance = float(seller_sell_balance[0]["quantity"]) - match_quantity
            db_client.delete("user_lot", f"number = '{seller_sell_balance[0]['number']}'")
            db_client.insert("user_lot", f"{seller_id}, {lot_to_buy}, {new_seller_sell_balance}")

            # Увеличиваем баланс у продавца (валюта продажи)
            seller_buy_balance = db_client.select("number, quantity", "user_lot", f"user_id = {seller_id} AND lot_id = {lot_to_sell}")
            new_seller_buy_balance = float(seller_buy_balance[0]["quantity"]) + (match_quantity * price)
            db_client.delete("user_lot", f"number = '{seller_buy_balance[0]['number']}'")
            db_client.insert("user_lot", f"{seller_id}, {lot_to_sell}, {new_seller_buy_balance}")

            # Обновляем существующий ордер
            new_quantity = float(existing_order["quantity"]) - match_quantity
            if new_quantity <= 0:
                db_client.delete("order", f"order_id = '{existing_order['order_id']}'")
                db_client.insert("order", f"{existing_order['user_id']}, {pair_id}, 0, {price}, '{reverse_order_type}', 'closed'")
            else:
                db_client.delete("order", f"order_id = '{existing_order['order_id']}'")
                db_client.insert("order", f"{existing_order['user_id']}, {pair_id}, {new_quantity}, {price}, '{reverse_order_type}', '0'")

            remaining_quantity -= match_quantity
            if remaining_quantity <= 0:
                break

        # Если осталась неудовлетворённая часть, создаём новый ордер
        if remaining_quantity > 0:
            db_client.insert("order", f"{user_id}, {pair_id}, {remaining_quantity}, {price}, '{order_type}', '0'")

        return jsonify({
            "status": "Order created successfully",
            "executed_quantity": quantity - remaining_quantity,
            "remaining_quantity": remaining_quantity
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500



@app.route('/config', methods=['POST'])
def load_config_endpoint():
    try:
        config_file = request.json.get('config_file', 'config.json')
        result = load_config(config_file)
        return jsonify({"status": "success", "message": "Configuration loaded successfully.", "details": result})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

def load_config(file_path):
    try:
        with open(file_path, 'r') as f:
            config = json.load(f)

        lots = config["lots"]
        lot_ids = []

        for lot_name in lots:
            db_client.insert("lot", f"'{lot_name}'")
            last_inserted_lot = db_client.select("*", "lot")
            if last_inserted_lot:
                lot_ids.append(int(last_inserted_lot[-1]["lot_id"]))

        pairs_inserted = 0
        for i in range(len(lot_ids)):
            for j in range(i + 1, len(lot_ids)):
                db_client.insert("pair", f"{lot_ids[i]}, {lot_ids[j]}")
                pairs_inserted += 1

        return {"lots_created": len(lots), "pairs_created": pairs_inserted}
    except Exception as e:
        raise RuntimeError(f"Error loading config: {str(e)}")

if __name__ == '__main__':
    app.run(debug=True)
