import sqlite3

conn = sqlite3.connect('store.db')
cursor = conn.cursor()

# Загружаем и выполняем SQL-файл с товарами
with open('seed_products.sql', 'r', encoding='utf-8') as f:
    cursor.executescript(f.read())

conn.commit()
print("Товары успешно добавлены!")
conn.close()