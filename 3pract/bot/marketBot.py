#запуск python3 market.py your_user_key

import requests
import random
import time
import sys

API_URL = "http://localhost:5000"  # URL вашего API

def place_order(user_key, order_type, pair_id, quantity, price):
    order_data = {
        "pair_id": pair_id,
        "quantity": quantity,
        "price": price,
        "type": order_type
    }
    headers = {
        "X-USER-KEY": user_key
    }
    response = requests.post(f"{API_URL}/order", json=order_data, headers=headers)
    return response.json()

def random_trading_bot(user_key):
    while True:
        order_type = random.choice(["buy", "sell"])
        pair_id = random.randint(1, 3)  # Диапазон ID пар
        quantity = random.uniform(10, 100)  # Случайное количество от 1 до 10
        price = random.uniform(0.001, 0.01)  # Случайная цена

        result = place_order(user_key, order_type, pair_id, quantity, price)
        print(f"Placed {order_type} order: {result}")

        time.sleep(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 marketBot.py <USER_KEY>")
        sys.exit(1)

    user_key = str(sys.argv[1])  # Преобразование ключа пользователя в строковое значение
    random_trading_bot(user_key)