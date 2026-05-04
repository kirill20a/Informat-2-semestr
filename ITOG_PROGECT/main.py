import sqlite3
import tkinter as tk
from tkinter import ttk, messagebox
from datetime import datetime

# Подключение к БД
conn = sqlite3.connect('cafe.db')
cursor = conn.cursor()

def init_db():
    """Инициализация базы данных Кафе"""
    cursor.executescript('''
        -- Таблица столиков
        CREATE TABLE IF NOT EXISTS tables_cafe (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            table_number INTEGER NOT NULL UNIQUE,
            capacity INTEGER DEFAULT 4,
            is_occupied INTEGER DEFAULT 0
        );
                         
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

        -- Таблица категорий блюд
        CREATE TABLE IF NOT EXISTS categories (
            id_category INTEGER PRIMARY KEY AUTOINCREMENT,
            name_category TEXT NOT NULL UNIQUE
        );

        -- Таблица меню (блюда)
        CREATE TABLE IF NOT EXISTS menu (
            id_dish INTEGER PRIMARY KEY AUTOINCREMENT,
            name_of_dish TEXT NOT NULL,
            price REAL NOT NULL CHECK (price >= 0),
            id_category INTEGER NOT NULL,
            cooking_time INTEGER DEFAULT 10,
            is_available INTEGER DEFAULT 1,
            FOREIGN KEY (id_category) REFERENCES categories(id_category)
        );

        -- Таблица продуктов на складе
        CREATE TABLE IF NOT EXISTS products (
            id_product INTEGER PRIMARY KEY AUTOINCREMENT,
            name_of_product TEXT NOT NULL,
            unit TEXT NOT NULL,
            quantity_at_storage REAL NOT NULL CHECK (quantity_at_storage >= 0),
            cost_per_unit REAL DEFAULT 0
        );

        -- Таблица состава блюда (техкарта)
        CREATE TABLE IF NOT EXISTS dish_composition (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_dish INTEGER NOT NULL,
            id_product INTEGER NOT NULL,
            quantity REAL NOT NULL CHECK (quantity > 0),
            FOREIGN KEY (id_dish) REFERENCES menu(id_dish),
            FOREIGN KEY (id_product) REFERENCES products(id_product)
        );

        -- Таблица заказов (чеки)
        CREATE TABLE IF NOT EXISTS orders (
            id_order INTEGER PRIMARY KEY AUTOINCREMENT,
            table_num INTEGER NOT NULL,
            id_employee INTEGER NOT NULL,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            status TEXT DEFAULT 'Открыт',
            FOREIGN KEY (id_employee) REFERENCES employees(id)
        );

        -- Таблица позиций в заказе
        CREATE TABLE IF NOT EXISTS order_items (
            id_item INTEGER PRIMARY KEY AUTOINCREMENT,
            id_order INTEGER NOT NULL,
            id_dish INTEGER NOT NULL,
            quantity INTEGER NOT NULL CHECK (quantity > 0),
            ready_status INTEGER DEFAULT 0,
            FOREIGN KEY (id_order) REFERENCES orders(id_order),
            FOREIGN KEY (id_dish) REFERENCES menu(id_dish)
        );
    ''')
    
    # Проверяем, есть ли начальные данные
    cursor.execute("SELECT COUNT(*) FROM job_titles")
    if cursor.fetchone()[0] == 0:
        cursor.executescript('''
            INSERT INTO tables_cafe (table_number, capacity) VALUES 
                (1, 2), (2, 2), (3, 4), (4, 4), (5, 4),
                (6, 6), (7, 6), (8, 2), (9, 4), (10, 4);
            INSERT INTO job_titles (name) VALUES 
                ('Бариста'), ('Официант'), ('Повар'), ('Администратор'), ('Управляющий');
            
            INSERT INTO employees (name, surname, id_job_title) VALUES 
                ('Иван', 'Иванов', 2),
                ('Анна', 'Петрова', 2),
                ('Алексей', 'Сидоров', 1),
                ('Елена', 'Козлова', 4),
                ('Павел', 'Смирнов', 3);
            
            INSERT INTO categories (name_category) VALUES 
                ('Салаты'), ('Супы'), ('Горячее'), ('Напитки'), ('Десерты');
            
            INSERT INTO menu (name_of_dish, price, id_category, cooking_time) VALUES 
                ('Цезарь с курицей', 280.00, 1, 10),
                ('Греческий салат', 220.00, 1, 7),
                ('Борщ', 220.00, 2, 15),
                ('Куриный суп', 180.00, 2, 12),
                ('Стейк из говядины', 450.00, 3, 20),
                ('Паста Карбонара', 320.00, 3, 15),
                ('Американо', 120.00, 4, 3),
                ('Капучино', 150.00, 4, 3),
                ('Лимонад', 120.00, 4, 2),
                ('Чизкейк', 200.00, 5, 3),
                ('Тирамису', 220.00, 5, 3);
            
            INSERT INTO products (name_of_product, unit, quantity_at_storage, cost_per_unit) VALUES 
                ('Куриное филе', 'кг', 5.0, 280.00),
                ('Говядина', 'кг', 3.0, 600.00),
                ('Салат Айсберг', 'кг', 2.0, 350.00),
                ('Помидоры черри', 'кг', 1.5, 200.00),
                ('Сыр Пармезан', 'кг', 1.0, 900.00),
                ('Кофе зерновой', 'кг', 1.0, 1200.00),
                ('Молоко', 'л', 10.0, 70.00),
                ('Свекла', 'кг', 4.0, 40.00),
                ('Капуста', 'кг', 5.0, 45.00),
                ('Лимоны', 'кг', 2.0, 180.00),
                ('Сахар', 'кг', 5.0, 55.00);
            
            -- Техкарты (состав блюд)
            INSERT INTO dish_composition (id_dish, id_product, quantity) VALUES 
                -- Цезарь с курицей
                (1, 1, 0.150), (1, 3, 0.050), (1, 4, 0.050), (1, 5, 0.030),
                -- Стейк Рибай
                (5, 2, 0.300),
                -- Американо
                (7, 6, 0.018),
                -- Капучино
                (8, 6, 0.018), (8, 7, 0.150),
                -- Борщ
                (3, 8, 0.100), (3, 9, 0.100);
        ''')
    
    conn.commit()

# GUI приложение для Кафе
class CafeApp:
    def refresh_top5(self, text_widget):
        text_widget.delete(1.0, tk.END)
        cursor.execute('''
            SELECT m.name_of_dish, SUM(oi.quantity)
            FROM order_items oi
            JOIN menu m ON oi.id_dish = m.id_dish
            JOIN orders o ON oi.id_order = o.id_order
            WHERE o.status IN ('Оплачен', 'Выполнен')
            GROUP BY m.id_dish ORDER BY SUM(oi.quantity) DESC LIMIT 5
        ''')
        text_widget.insert(tk.END, "=== ТОП-5 САМЫХ ПОПУЛЯРНЫХ БЛЮД ===\n\n")
        for i, (name, qty) in enumerate(cursor.fetchall(), 1):
            if qty:
                text_widget.insert(tk.END, f"{i}. {name}: {qty} порций\n")

    def __init__(self, root):
        self.root = root
        self.root.title("Кафе: Система обслуживания")
        self.root.geometry("750x650")
        self.root.configure(bg='#f0f0f0')
        
        self.cart = []            # Текущий заказ: список (id_dish, qty, name)
        self.current_table = 1    # Номер столика по умолчанию
        self.employee_id = 1      # По умолчанию официант Иванов
        
        self.build_widgets()

    def free_table(self):
        table_str = self.table_combo.get().replace(" (занят)", "")
        table_num = int(table_str)
        cursor.execute("SELECT is_occupied FROM tables_cafe WHERE table_number = ?", (table_num,))
        result = cursor.fetchone()
        if result and result[0] == 0:
            messagebox.showinfo("Инфо", f"Столик №{table_num} уже свободен")
            return
        if messagebox.askyesno("Освободить", f"Освободить столик №{table_num}?"):
            cursor.execute("UPDATE tables_cafe SET is_occupied = 0 WHERE table_number = ?", (table_num,))
            conn.commit()
            self.load_free_tables()
            messagebox.showinfo("Готово", f"Столик №{table_num} свободен")

    def load_free_tables(self):
        cursor.execute("SELECT table_number, is_occupied FROM tables_cafe ORDER BY table_number")
        all_tables = cursor.fetchall()
        
        free_list = []
        for num, occupied in all_tables:
            if occupied:
                free_list.append(f"{num} (занят)")
            else:
                free_list.append(str(num))
        
        self.table_combo['values'] = free_list
        # Выбираем первый свободный
        for num, occupied in all_tables:
            if not occupied:
                self.table_combo.set(str(num))
                break
    def build_widgets(self):
        # ===== ШАПКА =====
        header = tk.Frame(self.root, bg='#4a7c59', height=60)
        header.pack(fill='x')
        tk.Label(header, text="🍽️  КАФЕ «ВКУСНОЕ МЕСТО»", font=('Arial', 16, 'bold'), 
                 bg='#4a7c59', fg='white').pack(pady=12)
        
        # ===== ВЫБОР СТОЛИКА =====
        table_frame = tk.Frame(self.root, bg='#f0f0f0')
        table_frame.pack(fill='x', padx=15, pady=10)
        
        tk.Label(table_frame, text="Столик №:", font=('Arial', 11), bg='#f0f0f0').pack(side='left')
        self.table_combo = ttk.Combobox(table_frame, state="readonly", width=5, font=('Arial', 12))
        self.table_combo.pack(side='left', padx=5)
        self.load_free_tables()
        
        tk.Label(table_frame, text="Официант:", font=('Arial', 11), bg='#f0f0f0').pack(side='left', padx=(20, 5))
        self.waiter_combo = ttk.Combobox(table_frame, state="readonly", width=30)
        self.waiter_combo.pack(side='left', padx=5)
        self.load_waiters()
        
        ttk.Button(table_frame, text="🆕 Новый заказ", command=self.new_order).pack(side='right', padx=10)
        
        # ===== ВЫБОР БЛЮДА =====
        dish_frame = tk.LabelFrame(self.root, text="📋 Меню", font=('Arial', 11, 'bold'), 
                                   bg='#f0f0f0', fg='#333')
        dish_frame.pack(fill='x', padx=15, pady=5)
        
        inner_frame = tk.Frame(dish_frame, bg='#f0f0f0')
        inner_frame.pack(padx=10, pady=10)
        
        tk.Label(inner_frame, text="Блюдо:", font=('Arial', 10), bg='#f0f0f0').grid(row=0, column=0)
        self.dish_combo = ttk.Combobox(inner_frame, state="readonly", width=40)
        self.dish_combo.grid(row=0, column=1, padx=5)
        
        tk.Label(inner_frame, text="Кол-во:", font=('Arial', 10), bg='#f0f0f0').grid(row=0, column=2, padx=(10, 0))
        self.qty_entry = ttk.Entry(inner_frame, width=6, font=('Arial', 11))
        self.qty_entry.grid(row=0, column=3, padx=5)
        self.qty_entry.insert(0, "1")
        
        ttk.Button(inner_frame, text="➕ Добавить", command=self.add_to_cart).grid(row=0, column=4, padx=15)
        
        self.load_menu()
        
        # ===== КОРЗИНА =====
        cart_frame = tk.LabelFrame(self.root, text="🧾 Текущий заказ", font=('Arial', 11, 'bold'), 
                                   bg='#f0f0f0', fg='#333')
        cart_frame.pack(fill='both', expand=True, padx=15, pady=5)
        
        self.cart_listbox = tk.Listbox(cart_frame, width=80, height=10, font=('Consolas', 10))
        self.cart_listbox.pack(side='left', fill='both', expand=True, padx=5, pady=5)
        
        scrollbar = tk.Scrollbar(cart_frame, command=self.cart_listbox.yview)
        scrollbar.pack(side='right', fill='y')
        self.cart_listbox.config(yscrollcommand=scrollbar.set)
        
        # ===== КНОПКИ ДЕЙСТВИЙ =====
        action_frame = tk.Frame(self.root, bg='#f0f0f0')
        action_frame.pack(fill='x', padx=15, pady=10)
        
        ttk.Button(action_frame, text="💰 Оплатить (Закрыть чек)", 
                   command=self.checkout).pack(side='left', padx=5)
        ttk.Button(action_frame, text="🆓 Освободить столик", 
                   command=self.free_table).pack(side='left', padx=5)
        ttk.Button(action_frame, text="❌ Отменить заказ", 
                   command=self.cancel_order).pack(side='left', padx=5)
        
        # ===== КНОПКИ ДОП. ОКОН =====
        windows_frame = tk.Frame(self.root, bg='#f0f0f0')
        windows_frame.pack(fill='x', padx=15, pady=(0, 10))
        
        ttk.Button(windows_frame, text="👨‍🍳 Экран кухни", 
                   command=self.show_kitchen_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📦 Склад продуктов", 
                   command=self.show_stock_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📊 Отчёты", 
                   command=self.show_reports_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📋 Все заказы", 
                   command=self.show_orders_window).pack(side='left', padx=5)
    
    # ================================================================
    #  ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ЗАГРУЗКИ ДАННЫХ
    # ================================================================
    def load_waiters(self):
        """Загружает список официантов в выпадающий список"""
        cursor.execute("""
            SELECT e.id, e.name || ' ' || e.surname 
            FROM employees e 
            JOIN job_titles j ON e.id_job_title = j.id 
            WHERE j.name = 'Официант'
        """)
        waiters = cursor.fetchall()
        self.waiters_dict = {name: wid for wid, name in waiters}
        self.waiter_combo['values'] = list(self.waiters_dict.keys())
        if self.waiters_dict:
            self.waiter_combo.set(list(self.waiters_dict.keys())[0])
    
    def load_menu(self):
        """Загружает доступные блюда в выпадающий список"""
        cursor.execute("SELECT id_dish, name_of_dish FROM menu WHERE is_available = 1")
        dishes = cursor.fetchall()
        self.dishes_dict = {name: did for did, name in dishes}
        self.dish_combo['values'] = list(self.dishes_dict.keys())
        if self.dishes_dict:
            self.dish_combo.set(list(self.dishes_dict.keys())[0])
    
    # ================================================================
    #  ЛОГИКА ЗАКАЗА
    # ================================================================
    def new_order(self):
        if self.cart and not messagebox.askyesno("Новый заказ", "Текущий заказ будет потерян. Продолжить?"):
            return
        self.cart = []
        self.refresh_cart_display()
    
    def add_to_cart(self):
        dish_name = self.dish_combo.get()
        if not dish_name:
            messagebox.showwarning("Внимание", "Выберите блюдо из меню")
            return
        
        try:
            qty = int(self.qty_entry.get())
            if qty <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Ошибка", "Количество должно быть целым положительным числом")
            return
        
        dish_id = self.dishes_dict[dish_name]
        
        # Если такое блюдо уже есть в заказе — увеличиваем количество
        for item in self.cart:
            if item[0] == dish_id:
                item[1] += qty
                self.refresh_cart_display()
                return
        
        self.cart.append([dish_id, qty, dish_name])
        self.refresh_cart_display()
    
    def refresh_cart_display(self):
        self.cart_listbox.delete(0, tk.END)
        total = 0
        
        for dish_id, qty, name in self.cart:
            cursor.execute("SELECT price FROM menu WHERE id_dish = ?", (dish_id,))
            price = cursor.fetchone()[0]
            subtotal = price * qty
            total += subtotal
            self.cart_listbox.insert(tk.END, f"{name:35} {qty:3} x {price:8.2f} = {subtotal:8.2f} ₽")
        
        self.cart_listbox.insert(tk.END, "─" * 65)
        self.cart_listbox.insert(tk.END, f"{'ИТОГО:':>40} {total:15.2f} ₽")
    
    def cancel_order(self):
        self.cart = []
        self.refresh_cart_display()
        messagebox.showinfo("Отмена", "Заказ отменён")
    
    def checkout(self):
        if not self.cart:
            messagebox.showwarning("Корзина пуста", "Добавьте блюда в заказ")
            return
        table_str = self.table_combo.get().replace(" (занят)", "")
        if not table_str.isdigit():
            messagebox.showwarning("Ошибка", "Выберите свободный столик")
            return
        table_num = int(table_str)

        cursor.execute("SELECT is_occupied FROM tables_cafe WHERE table_number = ?", (table_num,))
        result = cursor.fetchone()
        if result and result[0] == 1:
            messagebox.showwarning("Столик занят", f"Столик №{table_num} уже занят! Выберите другой.")
            return
        # Проверяем наличие продуктов для всех блюд
        shortages = self.check_products_availability()
        if shortages:
            self.handle_shortages(shortages)
        else:
            self.complete_order()

    def check_products_availability(self):
        """Проверяет, хватит ли продуктов на складе"""
        needed = {}
        for dish_id, qty, _ in self.cart:
            cursor.execute("SELECT id_product, quantity FROM dish_composition WHERE id_dish = ?", (dish_id,))
            for prod_id, qty_per_portion in cursor.fetchall():
                needed[prod_id] = needed.get(prod_id, 0) + (qty_per_portion * qty)
        
        shortages = []
        for prod_id, total_needed in needed.items():
            cursor.execute("SELECT name_of_product, quantity_at_storage FROM products WHERE id_product = ?", (prod_id,))
            name, stock = cursor.fetchone()
            if stock < total_needed:
                shortages.append((prod_id, name, stock, total_needed))
        
        return shortages
    
    def handle_shortages(self, shortages):
        """Окно при нехватке продуктов"""
        dialog = tk.Toplevel(self.root)
        dialog.title("⚠️ Недостаточно продуктов")
        dialog.geometry("450x300")
        dialog.configure(bg='#fff8e1')
        
        tk.Label(dialog, text="На складе не хватает:", font=('Arial', 11, 'bold'), 
                 bg='#fff8e1', fg='#c62828').pack(pady=10)
        
        for _, name, stock, needed in shortages:
            tk.Label(dialog, text=f"• {name}: есть {stock:.3f}, нужно {needed:.3f}", 
                     bg='#fff8e1', font=('Arial', 10)).pack()
        
        tk.Label(dialog, text="\nЗаказать недостающие продукты у поставщика?", 
                 bg='#fff8e1', font=('Arial', 10)).pack(pady=10)
        
        def restock_all():
            for pid, _, _, needed in shortages:
                restock_qty = needed * 1.5
                cursor.execute("UPDATE products SET quantity_at_storage = quantity_at_storage + ? WHERE id_product = ?",
                             (restock_qty, pid))
            conn.commit()
            messagebox.showinfo("Успех", "Продукты заказаны и доставлены на склад!")
            dialog.destroy()
            self.complete_order()
        
        btn_frame = tk.Frame(dialog, bg='#fff8e1')
        btn_frame.pack(pady=15)
        ttk.Button(btn_frame, text="✅ Заказать и продолжить", command=restock_all).pack(side='left', padx=10)
        ttk.Button(btn_frame, text="❌ Отменить", command=dialog.destroy).pack(side='left', padx=10)
    
    def complete_order(self):
        """Финальное оформление заказа: запись в БД, списание продуктов"""
        table_num = int(self.table_combo.get().replace(" (занят)", ""))
        waiter_name = self.waiter_combo.get()
        waiter_id = self.waiters_dict.get(waiter_name, 1)
        
        from datetime import datetime, timezone
        order_date = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        cursor.execute("INSERT INTO orders (table_num, id_employee, created_at, status) VALUES (?, ?, ?, 'Готовится')",
               (table_num, waiter_id, order_date))
        order_id = cursor.lastrowid

        cursor.execute("UPDATE tables_cafe SET is_occupied = 1 WHERE table_number = ?", (table_num,))
        receipt_lines = []
        total = 0
        
        for dish_id, qty, name in self.cart:
            cursor.execute("SELECT price FROM menu WHERE id_dish = ?", (dish_id,))
            price = cursor.fetchone()[0]
            subtotal = price * qty
            total += subtotal
            
            cursor.execute("INSERT INTO order_items (id_order, id_dish, quantity, ready_status) VALUES (?, ?, ?, 0)",
                           (order_id, dish_id, qty))
            
            # Списание продуктов по техкарте
            cursor.execute("SELECT id_product, quantity FROM dish_composition WHERE id_dish = ?", (dish_id,))
            for prod_id, qty_per_portion in cursor.fetchall():
                cursor.execute("UPDATE products SET quantity_at_storage = quantity_at_storage - ? WHERE id_product = ?",
                               (qty_per_portion * qty, prod_id))
            
            receipt_lines.append(f"{name:35} {qty:3} x {price:8.2f} = {subtotal:8.2f} ₽")
        
        conn.commit()
        
        receipt = f"ЧЕК №{order_id}\n{'═'*60}\n"
        receipt += f"Столик: {table_num}  |  Официант: {waiter_name}\n"
        receipt += f"Дата: {order_date}\n{'─'*60}\n"
        receipt += "\n".join(receipt_lines)
        receipt += f"\n{'─'*60}\n{'ИТОГО:':>40} {total:15.2f} ₽"
        
        messagebox.showinfo("✅ Заказ оплачен", receipt)
        
        self.cart = []
        self.refresh_cart_display()
        self.load_menu()
        self.load_free_tables()
    
    # ================================================================
    #  ОКНО КУХНИ
    # ================================================================
    def show_kitchen_window(self):
        kitchen = tk.Toplevel(self.root)
        kitchen.title("👨‍🍳 Экран кухни — Очередь заказов")
        kitchen.geometry("850x400")
        kitchen.configure(bg='#fafafa')
        
        tk.Label(kitchen, text="Блюда, ожидающие приготовления", font=('Arial', 13, 'bold'), 
                 bg='#fafafa').pack(pady=10)
        
        tree_frame = tk.Frame(kitchen, bg='#fafafa')
        tree_frame.pack(fill='both', expand=True, padx=15, pady=5)
        
        scrollbar = ttk.Scrollbar(tree_frame)
        scrollbar.pack(side='right', fill='y')
        
        self.kitchen_tree = ttk.Treeview(tree_frame, 
                                         columns=('Чек', 'Стол', 'Блюдо', 'Кол-во', 'Время'), 
                                         show='headings', height=12, 
                                         yscrollcommand=scrollbar.set)
        self.kitchen_tree.pack(fill='both', expand=True)
        scrollbar.config(command=self.kitchen_tree.yview)
        
        self.kitchen_tree.heading('Чек', text='№ Заказа')
        self.kitchen_tree.heading('Стол', text='Стол')
        self.kitchen_tree.heading('Блюдо', text='Блюдо')
        self.kitchen_tree.heading('Кол-во', text='Кол-во')
        self.kitchen_tree.heading('Время', text='Ожидание (мин)')
        
        self.kitchen_tree.column('Чек', width=80, anchor='center')
        self.kitchen_tree.column('Стол', width=60, anchor='center')
        self.kitchen_tree.column('Блюдо', width=280)
        self.kitchen_tree.column('Кол-во', width=70, anchor='center')
        self.kitchen_tree.column('Время', width=120, anchor='center')
        
        def refresh_kitchen():
            for item in self.kitchen_tree.get_children():
                self.kitchen_tree.delete(item)
            
            cursor.execute('''
                SELECT o.id_order, o.table_num, m.name_of_dish, oi.quantity, m.cooking_time
                FROM order_items oi
                JOIN orders o ON oi.id_order = o.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE oi.ready_status = 0 
                  AND o.status IN ('Готовится', 'Оплачен', 'Выполнен')
                ORDER BY o.created_at ASC
            ''')
            
            for row in cursor.fetchall():
                self.kitchen_tree.insert('', 'end', values=row)
        def mark_ready():
            selected = self.kitchen_tree.selection()
            if not selected:
                messagebox.showwarning("Не выбрано", "Выберите блюдо для отметки")
                return
            
            item = self.kitchen_tree.item(selected[0])
            order_id = item['values'][0]
            dish_name = item['values'][2]
            
            cursor.execute('''
                UPDATE order_items 
                SET ready_status = 1 
                WHERE id_order = ? AND id_dish = (SELECT id_dish FROM menu WHERE name_of_dish = ?)
            ''', (order_id, dish_name))

            cursor.execute('''
                SELECT COUNT(*) FROM order_items 
                WHERE id_order = ? AND ready_status = 0
            ''', (order_id,))
            not_ready = cursor.fetchone()[0]
            
            if not_ready == 0:
                # Все блюда готовы — меняем статус заказа и освобождаем столик
                cursor.execute("UPDATE orders SET status = 'Выполнен' WHERE id_order = ?", (order_id,))
                cursor.execute("""
                    UPDATE tables_cafe 
                    SET is_occupied = 0 
                    WHERE table_number = (SELECT table_num FROM orders WHERE id_order = ?)
                """, (order_id,))
                self.load_free_tables() 
            conn.commit()
            refresh_kitchen()
        
        btn_frame = tk.Frame(kitchen, bg='#fafafa')
        btn_frame.pack(pady=10)
        ttk.Button(btn_frame, text="🔄 Обновить", command=refresh_kitchen).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="✅ Отметить готовым", command=mark_ready).pack(side='left', padx=5)
        
        refresh_kitchen()
     
    # ================================================================
    #  ОКНО СКЛАДА (ИЗ ВАШЕГО ОРИГИНАЛЬНОГО КОДА, АДАПТИРОВАНО)
    # ================================================================
    def show_stock_window(self):
        """Окно просмотра остатков на складе"""
        stock_window = tk.Toplevel(self.root)
        stock_window.title("📦 Склад продуктов")
        stock_window.geometry("900x550")
        
        filter_frame = tk.Frame(stock_window, bg='#f5f5f5')
        filter_frame.pack(fill='x', padx=10, pady=5)
        
        tk.Label(filter_frame, text="Поиск:", bg='#f5f5f5').pack(side='left', padx=5)
        search_entry = ttk.Entry(filter_frame, width=30)
        search_entry.pack(side='left', padx=5)
        
        tree_frame = tk.Frame(stock_window)
        tree_frame.pack(fill='both', expand=True, padx=10, pady=5)
        
        tree_scroll = ttk.Scrollbar(tree_frame)
        tree_scroll.pack(side='right', fill='y')
        
        stock_tree = ttk.Treeview(tree_frame, 
                                   columns=('ID', 'Название', 'Ед.изм', 'Остаток', 'Себест.', 'Статус'),
                                   show='headings', yscrollcommand=tree_scroll.set, height=20)
        stock_tree.pack(fill='both', expand=True)
        tree_scroll.config(command=stock_tree.yview)
        
        stock_tree.heading('ID', text='ID')
        stock_tree.heading('Название', text='Название продукта')
        stock_tree.heading('Ед.изм', text='Ед.')
        stock_tree.heading('Остаток', text='Остаток')
        stock_tree.heading('Себест.', text='Себест. (руб)')
        stock_tree.heading('Статус', text='Статус')
        
        stock_tree.column('ID', width=50, anchor='center')
        stock_tree.column('Название', width=250)
        stock_tree.column('Ед.изм', width=60, anchor='center')
        stock_tree.column('Остаток', width=80, anchor='center')
        stock_tree.column('Себест.', width=100, anchor='center')
        stock_tree.column('Статус', width=140, anchor='center')
        
        def refresh_stock():
            for item in stock_tree.get_children():
                stock_tree.delete(item)
            
            search_text = search_entry.get()
            
            query = """
                SELECT id_product, name_of_product, unit, quantity_at_storage, cost_per_unit
                FROM products WHERE 1=1
            """
            params = []
            if search_text:
                query += " AND name_of_product LIKE ?"
                params.append(f'%{search_text}%')
            query += " ORDER BY quantity_at_storage ASC, name_of_product"
            
            cursor.execute(query, params)
            
            total_value = 0
            low_stock_count = 0
            
            for product in cursor.fetchall():
                pid, name, unit, qty, cost = product
                total_value += cost * qty
                
                if qty == 0:
                    status, tags = "🔴 Нет в наличии", ('out',)
                elif qty < 1.0:
                    status, tags = "🟡 Заканчивается", ('low',)
                    low_stock_count += 1
                else:
                    status, tags = "🟢 В наличии", ('ok',)
                
                stock_tree.insert('', 'end', 
                                  values=(pid, name, unit, f"{qty:.3f}", f"{cost:.2f}", status), 
                                  tags=tags)
            
            stock_tree.tag_configure('out', background='#ffcccc')
            stock_tree.tag_configure('low', background='#ffffcc')
            stock_tree.tag_configure('ok', background='#ccffcc')
            
            stats_label.config(text=f"Всего продуктов: {len(stock_tree.get_children())} | "
                                   f"Заканчивается: {low_stock_count} | "
                                   f"Общая себестоимость: {total_value:.2f} руб.")
        
        button_frame = tk.Frame(stock_window)
        button_frame.pack(fill='x', padx=10, pady=5)
        
        ttk.Button(button_frame, text="🔄 Обновить", command=refresh_stock).pack(side='left', padx=5)
        
        def manual_restock():
            selected = stock_tree.selection()
            if not selected:
                messagebox.showwarning("Не выбран товар", "Выберите продукт для пополнения")
                return
            
            item = stock_tree.item(selected[0])
            product_id = item['values'][0]
            product_name = item['values'][1]
            current_qty = item['values'][3]
            
            dialog = tk.Toplevel(stock_window)
            dialog.title(f"Пополнение: {product_name}")
            dialog.geometry("300x150")
            
            tk.Label(dialog, text=f"Текущий остаток: {current_qty}").pack(pady=10)
            tk.Label(dialog, text="Количество для добавления:").pack(pady=5)
            
            qty_entry = ttk.Entry(dialog, width=20)
            qty_entry.pack(pady=5)
            qty_entry.focus()
            
            def do_restock():
                try:
                    add_qty = float(qty_entry.get())
                    if add_qty <= 0:
                        messagebox.showerror("Ошибка", "Количество должно быть положительным")
                        return
                    cursor.execute("UPDATE products SET quantity_at_storage = quantity_at_storage + ? WHERE id_product = ?",
                                   (add_qty, product_id))
                    conn.commit()
                    messagebox.showinfo("Успех", f"Добавлено {add_qty} ед.")
                    dialog.destroy()
                    refresh_stock()
                except ValueError:
                    messagebox.showerror("Ошибка", "Введите корректное число")
            
            ttk.Button(dialog, text="✅ Пополнить", command=do_restock).pack(pady=10)
            qty_entry.bind('<Return>', lambda e: do_restock())
        
        ttk.Button(button_frame, text="📦 Пополнить выбранное", command=manual_restock).pack(side='left', padx=5)
        
        stats_label = tk.Label(stock_window, text="", font=('Arial', 10, 'bold'), bg='#f5f5f5')
        stats_label.pack(pady=5)
        
        search_entry.bind('<Return>', lambda e: refresh_stock())
        refresh_stock()
    
    # ================================================================
    #  ОКНО ОТЧЁТОВ
    # ================================================================
    def show_reports_window(self):
        reports = tk.Toplevel(self.root)
        reports.title("📊 Отчёты")
        reports.geometry("700x500")
        reports.configure(bg='#fafafa')
        
        tk.Label(reports, text="Отчёты кафе", font=('Arial', 14, 'bold'), bg='#fafafa').pack(pady=10)
        
        notebook = ttk.Notebook(reports)
        notebook.pack(fill='both', expand=True, padx=10, pady=5)
        
        # Вкладка "Выручка"
        tab1 = tk.Frame(notebook, bg='#fafafa')
        notebook.add(tab1, text="💰 Выручка")
        
        tk.Label(tab1, text="Выберите дату:", bg='#fafafa', font=('Arial', 10)).pack(pady=5)
        date_frame = tk.Frame(tab1, bg='#fafafa')
        date_frame.pack()
        date_entry = ttk.Entry(date_frame, width=15, font=('Arial', 11))
        date_entry.pack(side='left', padx=5)
        date_entry.insert(0, datetime.now().strftime('%Y-%m-%d'))
        
        text_revenue = tk.Text(tab1, width=70, height=18, font=('Consolas', 10))
        text_revenue.pack(padx=10, pady=10)
        
        def show_revenue():
            date_val = date_entry.get()
            text_revenue.delete(1.0, tk.END)
            
            cursor.execute('''
                SELECT SUM(m.price * oi.quantity) as rev, COUNT(DISTINCT o.id_order) as checks
                FROM orders o
                JOIN order_items oi ON o.id_order = oi.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE DATE(o.created_at) = ? AND o.status IN ('Оплачен', 'Выполнен')            ''', (date_val,))
            rev, checks = cursor.fetchone()
            
            text_revenue.insert(tk.END, f"=== ОТЧЁТ ПО ВЫРУЧКЕ ЗА {date_val} ===\n\n")
            text_revenue.insert(tk.END, f"Закрытых чеков: {checks or 0}\n")
            text_revenue.insert(tk.END, f"Общая выручка: {rev or 0:.2f} ₽\n\n")
            
            cursor.execute('''
                SELECT m.name_of_dish, SUM(oi.quantity), SUM(m.price * oi.quantity)
                FROM order_items oi
                JOIN orders o ON oi.id_order = o.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE DATE(o.created_at) = ? AND o.status IN ('Оплачен', 'Выполнен')                GROUP BY m.id_dish ORDER BY SUM(oi.quantity) DESC
            ''', (date_val,))
            
            text_revenue.insert(tk.END, "Проданные блюда:\n")
            for name, qty, s in cursor.fetchall():
                text_revenue.insert(tk.END, f"  • {name}: {qty} порц. на {s:.2f} ₽\n")
        
        ttk.Button(tab1, text="📈 Показать", command=show_revenue).pack(pady=5)
        
        # Вкладка "ТОП-5"
        tab2 = tk.Frame(notebook, bg='#fafafa')
        notebook.add(tab2, text="🏆 ТОП-5 блюд")
        
        text_top = tk.Text(tab2, width=70, height=20, font=('Consolas', 10))
        ttk.Button(tab2, text="🔄 Обновить", 
                   command=lambda: self.refresh_top5(text_top)).pack(pady=5)
        text_top.pack(padx=10, pady=10)
        self.refresh_top5(text_top)
        text_top.pack(padx=10, pady=10)
        self.refresh_top5(text_top)
        
        cursor.execute('''
            SELECT m.name_of_dish, SUM(oi.quantity)
            FROM order_items oi
            JOIN menu m ON oi.id_dish = m.id_dish
            GROUP BY m.id_dish ORDER BY SUM(oi.quantity) DESC LIMIT 5
        ''')
        
        text_top.insert(tk.END, "=== ТОП-5 САМЫХ ПОПУЛЯРНЫХ БЛЮД ===\n\n")
        for i, (name, qty) in enumerate(cursor.fetchall(), 1):
            text_top.insert(tk.END, f"{i}. {name}: {qty} порций\n")
    
    # ================================================================
    #  ОКНО ВСЕХ ЗАКАЗОВ
    # ================================================================
    def show_orders_window(self):
        orders_win = tk.Toplevel(self.root)
        orders_win.title("📋 Все заказы")
        orders_win.geometry("800x400")
        
        tree = ttk.Treeview(orders_win, columns=('ID', 'Стол', 'Дата', 'Статус'), 
                            show='headings', height=15)
        tree.pack(fill='both', expand=True, padx=10, pady=10)
        
        tree.heading('ID', text='№ Заказа')
        tree.heading('Стол', text='Стол')
        tree.heading('Дата', text='Дата и время')
        tree.heading('Статус', text='Статус')
        
        tree.column('ID', width=80, anchor='center')
        tree.column('Стол', width=60, anchor='center')
        tree.column('Дата', width=200, anchor='center')
        tree.column('Статус', width=100, anchor='center')
        
        def refresh_orders():
            for item in tree.get_children():
                tree.delete(item)
            cursor.execute('''
                SELECT id_order, table_num, created_at, status 
                FROM orders ORDER BY id_order DESC LIMIT 50
            ''')
            for row in cursor.fetchall():
                tree.insert('', 'end', values=row)
        ttk.Button(orders_win, text="🔄 Обновить", command=refresh_orders).pack(pady=5)
        refresh_orders()
# ========== ЗАПУСК ==========
if __name__ == '__main__':
    init_db()
    root = tk.Tk()
    app = CafeApp(root)
    root.mainloop()
    conn.close()