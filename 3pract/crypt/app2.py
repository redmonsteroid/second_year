from flask import Flask, request, jsonify # type: ignore
from database_client import DatabaseClient
import random
import json

app = Flask(__name__)
db_client = DatabaseClient()  


def generate_user_key():
    """Генерирует уникальный числовой ключ пользователя."""
    return str(random.randint(1000000000, 9999999999))


@app.route('/')
def home():
    """Тестовый эндпоинт."""
    return "Crypto Exchange API is running!"


@app.route('/select', methods=['GET'])
def select_data():
    """Выполняет запрос SELECT через API."""
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
    """Создание нового пользователя с инициализацией баланса."""
    data = request.json
    username = data.get('username')
    if not username:
        return jsonify({"error": "no parametr 'username'"}), 400

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

        return jsonify({"status": "User created", "key": user_key, "db_response": user_response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route('/pairs', methods=['GET'])
def get_pairs():
    """Возвращает список валютных пар."""
    try:
        response = db_client.select("*", "pair")
        return jsonify({"status": "success", "data": response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route('/lots', methods=['GET'])
def get_lots():
    """Возвращает список лотов."""
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
    order_type = data.get('type')

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

        required_amount = quantity * price if order_type == "buy" else quantity

        # Получаем текущий баланс пользователя
        user_balance = db_client.select("number, quantity", "user_lot", f"user_id = {user_id} AND lot_id = {lot_to_sell}")
        if not user_balance or float(user_balance[0]["quantity"]) < required_amount:
            return jsonify({"error": "Insufficient balance"}), 400

        current_balance = float(user_balance[0]["quantity"])
        number_to_delete = user_balance[0]["number"]

        # Поиск обратных ордеров
        reverse_order_type = 'sell' if order_type == 'buy' else 'buy'
        existing_orders = db_client.select("*", "order", f"pair_id = {pair_id} AND type = '{reverse_order_type}' AND closed = '0'")

        remaining_quantity = quantity

        for existing_order in existing_orders:
            match_quantity = min(float(existing_order["quantity"]), remaining_quantity)

            # Обновляем баланс для продавца и покупателя
            seller_id = existing_order["user_id"] if order_type == 'buy' else user_id
            buyer_id = user_id if order_type == 'buy' else existing_order["user_id"]

            # Обновляем баланс продавца
            # Покупатель: уменьшаем валюту продажи (ETH) и увеличиваем актив (BTC)
            buyer_sell_balance = db_client.select("number, quantity", "user_lot", f"user_id = {buyer_id} AND lot_id = {lot_to_sell}")
            new_buyer_sell_balance = float(buyer_sell_balance[0]["quantity"]) - (match_quantity * price)
            db_client.delete("user_lot", f"number = '{buyer_sell_balance[0]['number']}'")
            db_client.insert("user_lot", f"{buyer_id}, {lot_to_sell}, {new_buyer_sell_balance}")

            buyer_buy_balance = db_client.select("number, quantity", "user_lot", f"user_id = {buyer_id} AND lot_id = {lot_to_buy}")
            new_buyer_buy_balance = float(buyer_buy_balance[0]["quantity"]) + match_quantity
            db_client.delete("user_lot", f"number = '{buyer_buy_balance[0]['number']}'")
            db_client.insert("user_lot", f"{buyer_id}, {lot_to_buy}, {new_buyer_buy_balance}")

# Продавец: уменьшаем актив (BTC) и увеличиваем валюту продажи (ETH)
            seller_sell_balance = db_client.select("number, quantity", "user_lot", f"user_id = {seller_id} AND lot_id = {lot_to_buy}")
            new_seller_sell_balance = float(seller_sell_balance[0]["quantity"]) - match_quantity
            db_client.delete("user_lot", f"number = '{seller_sell_balance[0]['number']}'")
            db_client.insert("user_lot", f"{seller_id}, {lot_to_buy}, {new_seller_sell_balance}")

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


@app.route('/order', methods=['DELETE'])
def delete_order():
    """Удаление ордера."""
    data = request.json
    order_id = data.get('order_id')

    if not order_id:
        return jsonify({"error": "Missing 'order_id' parameter"}), 400

    try:
        existing_order = db_client.select("*", "order", f"order_id = {order_id}")
        if not existing_order:
            return jsonify({"error": "Order not found"}), 404

        response = db_client.delete("order", f"order_id = {order_id}")
        return jsonify({"status": "Order deleted", "db_response": response})
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route('/balance', methods=['GET'])
def get_balance():
    user_key = request.headers.get('X-USER-KEY')

    if not user_key:
        return jsonify({"error": "User key is missing"}), 400

    try:
        user = db_client.select("user_id", "user", f"key = '{user_key}'")
        if not user:
            return jsonify({"error": "User not found"}), 404
        user_id = user[0]["user_id"]

        # Получаем текущий баланс из user_lot
        balances = db_client.select("*", "user_lot", f"user_id = {user_id}")
        balance_dict = {}
        for b in balances:
            lot_id = b["lot_id"]
            quantity = float(b["quantity"])
            balance_dict[lot_id] = quantity

        # Теперь получаем все открытые ордера пользователя
        open_orders = db_client.select("*", "order", f"user_id = {user_id} AND closed = '0'")
        for order in open_orders:
            pair_id = order["pair_id"]
            order_type = order["type"]
            order_qty = float(order["quantity"])
            order_price = float(order["price"])

            # Получаем информацию о паре
            pair = db_client.select("*", "pair", f"pair_id = {pair_id}")
            base_lot_id = pair[0]["first_lot_id"]
            quote_lot_id = pair[0]["second_lot_id"]

            if order_type == "buy":
                # Зарезервировано order_qty * order_price в quote_lot
                reserved_amount = order_qty * order_price
                if quote_lot_id in balance_dict:
                    balance_dict[quote_lot_id] -= reserved_amount
            else:
                # sell: зарезервировано order_qty base_lot_id
                reserved_amount = order_qty
                if base_lot_id in balance_dict:
                    balance_dict[base_lot_id] -= reserved_amount


        # Формируем ответ
        result = []
        for lot_id, qty in balance_dict.items():
            result.append({"lot_id": lot_id, "quantity": qty})

        return jsonify({"status": "success", "data": result})
    except Exception as e:
        return jsonify({"error": str(e)}), 500



@app.route('/config', methods=['POST'])
def load_config_endpoint():
    """Эндпоинт для загрузки конфигурации валют и пар."""
    try:
        config_file = request.json.get('config_file', 'config.json')
        result = load_config(config_file)
        return jsonify({"status": "success", "message": "Configuration loaded successfully.", "details": result})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500


def load_config(file_path):
    """Загружает конфигурацию валют и создаёт записи в таблицах lot и pair."""
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
    app.run(host='0.0.0.0', port=5000, debug=True)
