#запуск python3 user.py your_user_key

import requests
import random
import time
import sys

API_URL = "http://localhost:5000"  # URL вашего API

def get_price(pair_id):
    
    return random.uniform(0.001, 0.01)

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

def algorithmic_trading_bot(user_key):
    pair_id = 1  # Замените на реальный ID валютной пары
    previous_price = get_price(pair_id)

    while True:
        current_price = get_price(pair_id)
        print(f"Current price: {current_price}")

        if current_price > previous_price:
            result = place_order(user_key, "sell", pair_id, 1, current_price)  # Продаем 1 единицу
            print(f"Placed sell order: {result}")
        elif current_price < previous_price:
            result = place_order(user_key, "buy", pair_id, 1, current_price)  # Покупаем 1 единицу
            print(f"Placed buy order: {result}")

        previous_price = current_price
        time.sleep(3)  # Пауза между запросами

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 user.py <USER_KEY>")
        sys.exit(1)

    user_key = sys.argv[1]
    algorithmic_trading_bot(user_key)