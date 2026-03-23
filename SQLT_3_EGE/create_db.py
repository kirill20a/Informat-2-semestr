"""
Создание базы данных SQLite и импорт данных из Excel
"""

import sqlite3
import pandas as pd

def create_database():
    """Создание базы данных и импорт данных из Excel"""
    
    print("📁 Загрузка данных из Excel...")
    
    # Загружаем данные из Excel
    movements_df = pd.read_excel('3_ege.xlsx', sheet_name='Движение_товаров')
    products_df = pd.read_excel('3_ege.xlsx', sheet_name='Товар')
    shops_df = pd.read_excel('3_ege.xlsx', sheet_name='Магазин')
    
    # Приводим названия столбцов
    movements_df.columns = ['operation_id', 'date', 'shop_id', 'product_id', 
                            'quantity', 'operation_type', 'price']
    products_df.columns = ['product_id', 'department', 'product_name', 
                           'unit', 'quantity_per_pack', 'supplier']
    shops_df.columns = ['shop_id', 'district', 'address']
    
    print(f"   - Движение товаров: {len(movements_df)} записей")
    print(f"   - Товары: {len(products_df)} записей")
    print(f"   - Магазины: {len(shops_df)} записей")
    
    # Создаем базу данных SQLite
    print("\n🗄️ Создание базы данных SQLite...")
    conn = sqlite3.connect("store.db")
    
    # Импортируем данные в таблицы
    movements_df.to_sql('movements', conn, if_exists='replace', index=False)
    products_df.to_sql('products', conn, if_exists='replace', index=False)
    shops_df.to_sql('shops', conn, if_exists='replace', index=False)
    
    conn.close()
    
    print("✅ База данных 'store.db' создана успешно!")
    print("   Таблицы: movements, products, shops")

if __name__ == "__main__":
    create_database()