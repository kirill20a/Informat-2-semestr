import sqlite3
import tkinter as tk
from tkinter import ttk, messagebox
from datetime import datetime

# Подключение к БД
conn = sqlite3.connect('store.db')
cursor = conn.cursor()

def init_db():
    """Инициализация базы данных"""
    cursor.executescript('''
        -- Таблица должностей
        CREATE TABLE IF NOT EXISTS job_titles (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        );

        -- Таблица сотрудников
        CREATE TABLE IF NOT EXISTS employees (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            surname TEXT NOT NULL,
            id_job_title INTEGER NOT NULL,
            FOREIGN KEY (id_job_title) REFERENCES job_titles(id)
        );

        -- Таблица категорий
        CREATE TABLE IF NOT EXISTS categories (
            id_category INTEGER PRIMARY KEY AUTOINCREMENT,
            name_category TEXT NOT NULL UNIQUE
        );

        -- Таблица товаров
        CREATE TABLE IF NOT EXISTS products (
            id_product INTEGER PRIMARY KEY AUTOINCREMENT,
            name_of_product TEXT NOT NULL,
            price REAL NOT NULL CHECK (price >= 0),
            id_category INTEGER NOT NULL,
            quantity_at_storage REAL NOT NULL CHECK (quantity_at_storage >= 0),
            FOREIGN KEY (id_category) REFERENCES categories(id_category)
        );

        -- Таблица чеков
        CREATE TABLE IF NOT EXISTS receipts (
            id_check INTEGER PRIMARY KEY AUTOINCREMENT,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            id_cashier INTEGER NOT NULL,
            FOREIGN KEY (id_cashier) REFERENCES employees(id)
        );

        -- Таблица позиций в чеке
        CREATE TABLE IF NOT EXISTS sale_items (
            id_sale INTEGER PRIMARY KEY AUTOINCREMENT,
            id_check INTEGER NOT NULL,
            id_product INTEGER NOT NULL,
            quantity REAL NOT NULL CHECK (quantity > 0),
            price_at_sale REAL NOT NULL,
            FOREIGN KEY (id_check) REFERENCES receipts(id_check),
            FOREIGN KEY (id_product) REFERENCES products(id_product)
        );
    ''')
    
    # Проверяем, есть ли начальные данные
    cursor.execute("SELECT COUNT(*) FROM job_titles")
    if cursor.fetchone()[0] == 0:
        cursor.executescript('''
            INSERT INTO job_titles (name) VALUES ('Кассир'), ('Администратор');
            INSERT INTO employees (name, surname, id_job_title) VALUES ('Иван', 'Иванов', 1);
            INSERT INTO categories (name_category) VALUES 
                ('Молочные продукты'),
                ('Хлебобулочные изделия'),
                ('Напитки');
            INSERT INTO products (name_of_product, price, id_category, quantity_at_storage) VALUES 
                ('Молоко 3.2%', 89.90, 1, 25),
                ('Хлеб белый', 42.50, 2, 30),
                ('Вода 1.5л', 55.00, 3, 48);
        ''')
    
    conn.commit()

# GUI приложение
class StoreApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Магазин: Учет продаж")
        self.root.geometry("600x500")
        self.cart = []
        self.cashier_id = 1
        
        self.build_widgets()
    
    def build_widgets(self):
        # Выбор товара
        ttk.Label(self.root, text="Товар:").grid(row=0, column=0, padx=5, pady=5)
        self.product_combo = ttk.Combobox(self.root, state="readonly", width=30)
        self.product_combo.grid(row=0, column=1, padx=5, pady=5)
        self.load_products()
        
        ttk.Label(self.root, text="Количество:").grid(row=0, column=2, padx=5, pady=5)
        self.qty_entry = ttk.Entry(self.root, width=10)
        self.qty_entry.grid(row=0, column=3, padx=5, pady=5)
        
        ttk.Button(self.root, text="➕ Добавить в корзину", 
                   command=self.add_to_cart).grid(row=0, column=4, padx=5, pady=5)
        
        # Корзина
        ttk.Label(self.root, text="Корзина:", font=('Arial', 10, 'bold')).grid(row=1, column=0, columnspan=5, pady=5)
        self.cart_listbox = tk.Listbox(self.root, width=70, height=10)
        self.cart_listbox.grid(row=2, column=0, columnspan=5, padx=10, pady=5)
        
        ttk.Button(self.root, text="💰 Оформить покупку", 
                   command=self.checkout).grid(row=3, column=0, columnspan=5, pady=10)
        
        # Отчеты
        ttk.Label(self.root, text="Дата (ГГГГ-ММ-ДД):").grid(row=4, column=0, pady=5)
        self.date_entry = ttk.Entry(self.root)
        self.date_entry.grid(row=4, column=1, pady=5)
        
        ttk.Button(self.root, text="📈 Отчет по дате", 
                   command=self.report_by_date).grid(row=4, column=2, padx=5, pady=5)
        
        # Кнопка просмотра остатков
        ttk.Button(self.root, text="📊 Остатки на складе", 
                   command=self.show_stock_window).grid(row=5, column=0, columnspan=5, pady=10)
        
        self.report_text = tk.Text(self.root, height=8, width=70)
        self.report_text.grid(row=6, column=0, columnspan=5, padx=10, pady=5)
    
    def load_products(self):
        cursor.execute("SELECT id_product, name_of_product FROM products WHERE quantity_at_storage > 0")
        products = cursor.fetchall()
        self.products_dict = {name: pid for pid, name in products}
        self.product_combo['values'] = list(self.products_dict.keys())
        if self.products_dict:
            self.product_combo.set(list(self.products_dict.keys())[0])
    
    def add_to_cart(self):
        name = self.product_combo.get()
        qty = self.qty_entry.get()
        if not name or not qty:
            messagebox.showwarning("Внимание", "Выберите товар и укажите количество")
            return
        
        try:
            qty = float(qty)
            if qty <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Ошибка", "Количество должно быть положительным числом")
            return
        
        pid = self.products_dict[name]
        self.cart.append((pid, qty, name))
        self.cart_listbox.insert(tk.END, f"{name} — {qty} ед.")
        self.qty_entry.delete(0, tk.END)
    
    def checkout(self):
        if not self.cart:
            messagebox.showwarning("Корзина пуста", "Добавьте товары в корзину")
            return
        
        # Проверка наличия
        shortages = []
        for pid, qty, name in self.cart:
            cursor.execute('SELECT quantity_at_storage FROM products WHERE id_product = ?', (pid,))
            stock = cursor.fetchone()[0]
            if stock < qty:
                shortages.append((pid, name, stock, qty))
        
        if shortages:
            self.handle_shortages(shortages)
        else:
            self.complete_sale()
    
    def handle_shortages(self, shortages):
        """Обработка нехватки товара"""
        dialog = tk.Toplevel(self.root)
        dialog.title("⚠️ Недостаточно товара")
        dialog.geometry("400x300")
        
        ttk.Label(dialog, text="Не хватает следующих товаров:", 
                  font=('Arial', 10, 'bold')).pack(pady=10)
        
        for pid, name, stock, needed in shortages:
            ttk.Label(dialog, text=f"{name}: есть {stock}, нужно {needed}").pack()
        
        ttk.Label(dialog, text="\nПополнить склад?").pack(pady=10)
        
        def restock_all():
            for pid, name, stock, needed in shortages:
                restock_qty = needed * 2  # Заказываем с запасом
                cursor.execute("UPDATE products SET quantity_at_storage = quantity_at_storage + ? WHERE id_product = ?",
                             (restock_qty, pid))
            conn.commit()
            messagebox.showinfo("Успех", "Склад пополнен!")
            dialog.destroy()
            self.complete_sale()
        
        ttk.Button(dialog, text="✅ Пополнить и продолжить", 
                   command=restock_all).pack(pady=5)
        ttk.Button(dialog, text="❌ Отменить", 
                   command=dialog.destroy).pack(pady=5)
    
    def complete_sale(self):
        """Завершение продажи"""
        receipt_date = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        total_cost = 0
        
        cursor.execute('INSERT INTO receipts (created_at, id_cashier) VALUES (?, ?)',
                       (receipt_date, self.cashier_id))
        check_id = cursor.lastrowid
        
        receipt_text = f"ЧЕК №{check_id}\n{'='*40}\n{receipt_date}\n{'-'*40}\n"
        
        for pid, qty, name in self.cart:
            cursor.execute('SELECT price FROM products WHERE id_product = ?', (pid,))
            price = cursor.fetchone()[0]
            total_cost += price * qty
            
            cursor.execute('''
                INSERT INTO sale_items (id_check, id_product, quantity, price_at_sale)
                VALUES (?, ?, ?, ?)
            ''', (check_id, pid, qty, price))
            
            cursor.execute('UPDATE products SET quantity_at_storage = quantity_at_storage - ? WHERE id_product = ?',
                           (qty, pid))
            
            receipt_text += f"{name[:30]:30} {qty:5} x {price:7.2f} = {qty*price:7.2f}\n"
        
        conn.commit()
        receipt_text += f"{'-'*40}\nИТОГО: {total_cost:35.2f} руб."
        
        messagebox.showinfo("✅ Покупка оформлена", receipt_text)
        
        self.cart.clear()
        self.cart_listbox.delete(0, tk.END)
        self.load_products()
    
    def report_by_date(self):
        date = self.date_entry.get()
        if not date:
            messagebox.showwarning("Внимание", "Введите дату")
            return
        
        cursor.execute('''
            SELECT p.name_of_product, SUM(si.quantity) as total_qty, 
                   SUM(si.quantity * si.price_at_sale) as revenue
            FROM sale_items si
            JOIN products p ON si.id_product = p.id_product
            JOIN receipts r ON si.id_check = r.id_check
            WHERE DATE(r.created_at) = ?
            GROUP BY p.id_product
        ''', (date,))
        
        rows = cursor.fetchall()
        self.report_text.delete(1.0, tk.END)
        
        if not rows:
            self.report_text.insert(tk.END, f"Нет продаж за {date}")
            return
        
        total_revenue = 0
        self.report_text.insert(tk.END, f"Отчет за {date}\n{'='*50}\n")
        for name, qty, rev in rows:
            self.report_text.insert(tk.END, f"{name}: {qty} ед. — {rev:.2f} руб.\n")
            total_revenue += rev
        self.report_text.insert(tk.END, f"{'='*50}\nОбщая выручка: {total_revenue:.2f} руб.")
    
    # ========== МЕТОД ДЛЯ ПРОСМОТРА ОСТАТКОВ ==========
    def show_stock_window(self):
        """Окно просмотра остатков на складе"""
        stock_window = tk.Toplevel(self.root)
        stock_window.title("Остатки на складе")
        stock_window.geometry("900x550")
        
        # Фрейм для поиска и фильтрации
        filter_frame = ttk.Frame(stock_window)
        filter_frame.pack(fill='x', padx=10, pady=5)
        
        ttk.Label(filter_frame, text="Поиск:").pack(side='left', padx=5)
        search_entry = ttk.Entry(filter_frame, width=30)
        search_entry.pack(side='left', padx=5)
        
        ttk.Label(filter_frame, text="Категория:").pack(side='left', padx=5)
        category_filter = ttk.Combobox(filter_frame, state="readonly", width=20)
        category_filter.pack(side='left', padx=5)
        
        # Загружаем категории для фильтра
        cursor.execute("SELECT name_category FROM categories")
        categories = ['Все'] + [cat[0] for cat in cursor.fetchall()]
        category_filter['values'] = categories
        category_filter.set('Все')
        
        # Таблица для отображения остатков
        tree_frame = ttk.Frame(stock_window)
        tree_frame.pack(fill='both', expand=True, padx=10, pady=5)
        
        # Создаем Treeview с прокруткой
        tree_scroll = ttk.Scrollbar(tree_frame)
        tree_scroll.pack(side='right', fill='y')
        
        stock_tree = ttk.Treeview(tree_frame, 
                                   columns=('ID', 'Название', 'Категория', 'Цена', 'Остаток', 'Статус'),
                                   show='headings',
                                   yscrollcommand=tree_scroll.set,
                                   height=20)
        
        stock_tree.pack(fill='both', expand=True)
        tree_scroll.config(command=stock_tree.yview)
        
        # Настройка колонок
        stock_tree.heading('ID', text='ID')
        stock_tree.heading('Название', text='Название товара')
        stock_tree.heading('Категория', text='Категория')
        stock_tree.heading('Цена', text='Цена (руб)')
        stock_tree.heading('Остаток', text='Остаток')
        stock_tree.heading('Статус', text='Статус')
        
        stock_tree.column('ID', width=50)
        stock_tree.column('Название', width=250)
        stock_tree.column('Категория', width=150)
        stock_tree.column('Цена', width=100)
        stock_tree.column('Остаток', width=100)
        stock_tree.column('Статус', width=150)
        
        # Функция обновления таблицы
        def refresh_stock():
            # Очищаем таблицу
            for item in stock_tree.get_children():
                stock_tree.delete(item)
            
            # Формируем запрос с учетом фильтров
            search_text = search_entry.get()
            selected_category = category_filter.get()
            
            query = """
                SELECT p.id_product, p.name_of_product, c.name_category, 
                       p.price, p.quantity_at_storage
                FROM products p
                JOIN categories c ON p.id_category = c.id_category
                WHERE 1=1
            """
            params = []
            
            if search_text:
                query += " AND p.name_of_product LIKE ?"
                params.append(f'%{search_text}%')
            
            if selected_category != 'Все':
                query += " AND c.name_category = ?"
                params.append(selected_category)
            
            query += " ORDER BY p.quantity_at_storage ASC, p.name_of_product"
            
            cursor.execute(query, params)
            
            # Заполняем таблицу
            total_value = 0
            low_stock_count = 0
            
            for product in cursor.fetchall():
                pid, name, category, price, qty = product
                total_value += price * qty
                
                # Определяем статус
                if qty == 0:
                    status = "🔴 Нет в наличии"
                    tags = ('out_of_stock',)
                elif qty < 5:
                    status = "🟡 Заканчивается"
                    tags = ('low_stock',)
                    low_stock_count += 1
                else:
                    status = "🟢 В наличии"
                    tags = ('in_stock',)
                
                stock_tree.insert('', 'end', 
                                  values=(pid, name, category, f"{price:.2f}", qty, status),
                                  tags=tags)
            
            # Настройка цветов для статусов
            stock_tree.tag_configure('out_of_stock', background='#ffcccc')
            stock_tree.tag_configure('low_stock', background='#ffffcc')
            stock_tree.tag_configure('in_stock', background='#ccffcc')
            
            # Обновляем статистику
            stats_label.config(text=f"Всего товаров: {len(stock_tree.get_children())} | "
                                   f"Товаров заканчивается: {low_stock_count} | "
                                   f"Общая стоимость: {total_value:.2f} руб.")
        
        # Кнопки управления
        button_frame = ttk.Frame(stock_window)
        button_frame.pack(fill='x', padx=10, pady=5)
        
        ttk.Button(button_frame, text="🔄 Обновить", command=refresh_stock).pack(side='left', padx=5)
        ttk.Button(button_frame, text="🔍 Поиск", command=refresh_stock).pack(side='left', padx=5)
        
        # Кнопка для ручного пополнения склада
        def manual_restock():
            selected = stock_tree.selection()
            if not selected:
                messagebox.showwarning("Не выбран товар", "Выберите товар для пополнения")
                return
            
            item = stock_tree.item(selected[0])
            product_id = item['values'][0]
            product_name = item['values'][1]
            current_qty = item['values'][4]
            
            # Диалог для ввода количества
            dialog = tk.Toplevel(stock_window)
            dialog.title(f"Пополнение: {product_name}")
            dialog.geometry("300x150")
            
            ttk.Label(dialog, text=f"Текущий остаток: {current_qty}").pack(pady=10)
            ttk.Label(dialog, text="Количество для добавления:").pack(pady=5)
            
            qty_entry = ttk.Entry(dialog, width=20)
            qty_entry.pack(pady=5)
            qty_entry.focus()
            
            def do_restock():
                try:
                    add_qty = float(qty_entry.get())
                    if add_qty <= 0:
                        messagebox.showerror("Ошибка", "Количество должно быть положительным")
                        return
                    
                    cursor.execute("""
                        UPDATE products 
                        SET quantity_at_storage = quantity_at_storage + ? 
                        WHERE id_product = ?
                    """, (add_qty, product_id))
                    conn.commit()
                    
                    messagebox.showinfo("Успех", f"Добавлено {add_qty} ед. товара '{product_name}'")
                    dialog.destroy()
                    refresh_stock()
                    self.load_products()  # Обновляем список товаров в главном окне
                    
                except ValueError:
                    messagebox.showerror("Ошибка", "Введите корректное число")
            
            ttk.Button(dialog, text="✅ Пополнить", command=do_restock).pack(pady=10)
            qty_entry.bind('<Return>', lambda e: do_restock())
        
        ttk.Button(button_frame, text="📦 Пополнить выбранное", command=manual_restock).pack(side='left', padx=5)
        
        # Статистика
        stats_label = ttk.Label(stock_window, text="", font=('Arial', 10, 'bold'))
        stats_label.pack(pady=5)
        
        # Привязываем Enter к поиску
        search_entry.bind('<Return>', lambda e: refresh_stock())
        
        # Первоначальная загрузка
        refresh_stock()

# ========== ЗАПУСК ПРИЛОЖЕНИЯ ==========
if __name__ == '__main__':
    init_db()
    root = tk.Tk()
    app = StoreApp(root)
    root.mainloop()
    conn.close()

