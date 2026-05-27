import sqlite3
import tkinter as tk
from tkinter import ttk, messagebox
from datetime import datetime
import os

# Подключение к БД
conn = sqlite3.connect('cafe.db')
cursor = conn.cursor()

def init_db():
    import os
    
    conn = sqlite3.connect('cafe.db')
    cursor = conn.cursor()
    
    # Создаём таблицы
    with open('init_db.sql', 'r', encoding='utf-8') as f:
        cursor.executescript(f.read())
    
    # Загружаем данные из TXT
    txt_path = os.path.join('data', 'init_data.txt')
    
    if not os.path.exists(txt_path):
        conn.commit()
        conn.close()
        return
    
    with open(txt_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    sections = content.strip().split('\n\n')
    
    for section in sections:
        lines = section.strip().split('\n')
        if not lines:
            continue
        
        header = lines[0].strip()
        
        # Определяем таблицу
        if header == '# СТОЛИКИ':
            table = 'tables_cafe'
        elif header == '# ДОЛЖНОСТИ':
            table = 'job_titles'
        elif header == '# СОТРУДНИКИ':
            table = 'employees'
        elif header == '# КАТЕГОРИИ':
            table = 'categories'
        elif header == '# МЕНЮ':
            table = 'menu'
        elif header == '# ПРОДУКТЫ':
            table = 'products'
        elif header == '# СОСТАВ БЛЮД':
            table = 'dish_composition'
        else:
            continue
        
        # Проверяем, есть ли уже данные
        cursor.execute(f"SELECT COUNT(*) FROM {table}")
        if cursor.fetchone()[0] > 0:
            continue
        
        # Парсим столбцы и данные
        if len(lines) < 2:
            continue
        
        columns = lines[1].strip().split('|')
        
        for line in lines[2:]:
            values = line.strip().split('|')
            if len(values) == len(columns):
                placeholders = ','.join(['?' for _ in columns])
                cols = ','.join(columns)
                cursor.execute(f"INSERT INTO {table} ({cols}) VALUES ({placeholders})", values)
    
    conn.commit()
    conn.close()
    print("База данных загружена из data/init_data.txt")

# GUI приложение для Кафе
class CafeApp:
    def open_admin_products(self):
        self.prod_win = tk.Toplevel(self.root)
        self.prod_win.title("📦 Управление продуктами")
        self.prod_win.geometry("700x450")
        
        tk.Label(self.prod_win, text="Продукты на складе", font=('Arial', 13, 'bold')).pack(pady=10)
        
        tree_frame = tk.Frame(self.prod_win)
        tree_frame.pack(fill='both', expand=True, padx=10)
        sb = ttk.Scrollbar(tree_frame)
        sb.pack(side='right', fill='y')
        
        self.prod_tree = ttk.Treeview(tree_frame, 
                                       columns=('ID', 'Название', 'Ед.', 'Остаток', 'Себест.'),
                                       show='headings', height=12, yscrollcommand=sb.set)
        self.prod_tree.pack(fill='both', expand=True)
        sb.config(command=self.prod_tree.yview)
        
        for i, (col, w) in enumerate([('ID', 40), ('Название', 200), ('Ед.', 60), ('Остаток', 80), ('Себест.', 80)]):
            self.prod_tree.heading(col, text=col)
            self.prod_tree.column(col, width=w, anchor='center' if col != 'Название' else 'w')
        
        def refresh():
            for item in self.prod_tree.get_children():
                self.prod_tree.delete(item)
            cursor.execute("SELECT id_product, name_of_product, unit, quantity_at_storage, cost_per_unit FROM products ORDER BY id_product")
            for row in cursor.fetchall():
                self.prod_tree.insert('', 'end', values=row)
        
        form = tk.LabelFrame(self.prod_win, text="Добавить продукт")
        form.pack(fill='x', padx=10, pady=10)
        inner = tk.Frame(form)
        inner.pack(padx=10, pady=10)
        
        tk.Label(inner, text="Название:").grid(row=0, column=0)
        pname = ttk.Entry(inner, width=20)
        pname.grid(row=0, column=1, padx=5)
        tk.Label(inner, text="Ед.:").grid(row=0, column=2)
        punit = ttk.Entry(inner, width=6)
        punit.grid(row=0, column=3, padx=5)
        tk.Label(inner, text="Себест.:").grid(row=0, column=4)
        pcost = ttk.Entry(inner, width=10)
        pcost.grid(row=0, column=5, padx=5)
        
        def add():
            try:
                cursor.execute("INSERT INTO products (name_of_product, unit, cost_per_unit) VALUES (?, ?, ?)",
                              (pname.get(), punit.get(), float(pcost.get())))
                conn.commit()
                pname.delete(0, tk.END)
                punit.delete(0, tk.END)
                pcost.delete(0, tk.END)
                refresh()
            except ValueError:
                messagebox.showerror("Ошибка", "Неверная цена")
        
        ttk.Button(inner, text="➕", command=add).grid(row=0, column=6, padx=10)
        
        def delete():
            sel = self.prod_tree.selection()
            if sel:
                item = self.prod_tree.item(sel[0])
                if messagebox.askyesno("Удалить", "Удалить продукт?"):
                    cursor.execute("DELETE FROM products WHERE id_product=?", (item['values'][0],))
                    conn.commit()
                    refresh()
        
        ttk.Button(self.prod_win, text="🗑️ Удалить", command=delete).pack(pady=5)
        refresh()
    def open_admin_menu(self):
        self.admin_menu = tk.Toplevel(self.root)
        self.admin_menu.title("📝 Управление меню")
        self.admin_menu.geometry("750x500")
        self.admin_menu.configure(bg='#fafafa')
        
        tk.Label(self.admin_menu, text="Управление меню", font=('Arial', 14, 'bold'), bg='#fafafa').pack(pady=10)
        
        # Таблица
        tree_frame = tk.Frame(self.admin_menu)
        tree_frame.pack(fill='both', expand=True, padx=10, pady=5)
        
        scrollbar = ttk.Scrollbar(tree_frame)
        scrollbar.pack(side='right', fill='y')
        
        self.menu_tree = ttk.Treeview(tree_frame, 
                                       columns=('ID', 'Название', 'Цена', 'Категория', 'Время', 'Доступно'),
                                       show='headings', height=12, yscrollcommand=scrollbar.set)
        self.menu_tree.pack(fill='both', expand=True)
        scrollbar.config(command=self.menu_tree.yview)
        
        self.menu_tree.heading('ID', text='ID')
        self.menu_tree.heading('Название', text='Название')
        self.menu_tree.heading('Цена', text='Цена')
        self.menu_tree.heading('Категория', text='Категория')
        self.menu_tree.heading('Время', text='Время (мин)')
        self.menu_tree.heading('Доступно', text='Доступно')
        
        self.menu_tree.column('ID', width=40, anchor='center')
        self.menu_tree.column('Название', width=180)
        self.menu_tree.column('Цена', width=80, anchor='center')
        self.menu_tree.column('Категория', width=120, anchor='center')
        self.menu_tree.column('Время', width=80, anchor='center')
        self.menu_tree.column('Доступно', width=80, anchor='center')
        
        # Форма
        form = tk.LabelFrame(self.admin_menu, text="Добавить / Изменить", bg='#fafafa')
        form.pack(fill='x', padx=10, pady=10)
        inner = tk.Frame(form, bg='#fafafa')
        inner.pack(padx=10, pady=10)
        
        tk.Label(inner, text="Название:", bg='#fafafa').grid(row=0, column=0)
        self.am_name = ttk.Entry(inner, width=25)
        self.am_name.grid(row=0, column=1, padx=5)
        
        tk.Label(inner, text="Цена:", bg='#fafafa').grid(row=0, column=2)
        self.am_price = ttk.Entry(inner, width=10)
        self.am_price.grid(row=0, column=3, padx=5)
        
        tk.Label(inner, text="Время (мин):", bg='#fafafa').grid(row=0, column=4)
        self.am_time = ttk.Entry(inner, width=8)
        self.am_time.grid(row=0, column=5, padx=5)
        
        btn_inner = tk.Frame(form, bg='#fafafa')
        btn_inner.pack(pady=5)
        ttk.Button(btn_inner, text="➕ Добавить", command=self.admin_add_dish).pack(side='left', padx=5)
        ttk.Button(btn_inner, text="✏️ Изменить", command=self.admin_edit_dish).pack(side='left', padx=5)
        ttk.Button(btn_inner, text="🗑️ Удалить", command=self.admin_delete_dish).pack(side='left', padx=5)
        ttk.Button(btn_inner, text="🔄 Вкл/Выкл", command=self.admin_toggle_dish).pack(side='left', padx=5)
        
        self.admin_refresh_menu()
    
    def admin_refresh_menu(self):
        for item in self.menu_tree.get_children():
            self.menu_tree.delete(item)
        cursor.execute('''
            SELECT m.id_dish, m.name_of_dish, m.price, c.name_category, m.cooking_time, m.is_available
            FROM menu m JOIN categories c ON m.id_category = c.id_category ORDER BY m.id_dish
        ''')
        for row in cursor.fetchall():
            did, name, price, cat, time, avail = row
            self.menu_tree.insert('', 'end', values=(did, name, f"{price:.2f}", cat, time, '✅' if avail else '❌'))
    
    def admin_add_dish(self):
        name = self.am_name.get().strip()
        if not name:
            messagebox.showwarning("Ошибка", "Введите название")
            return
        try:
            price = float(self.am_price.get())
            time = int(self.am_time.get() or 10)
        except ValueError:
            messagebox.showerror("Ошибка", "Неверные числа")
            return
        cursor.execute("INSERT INTO menu (name_of_dish, price, id_category, cooking_time) VALUES (?, ?, 1, ?)",
                       (name, price, time))
        conn.commit()
        self.admin_refresh_menu()
        self.load_menu()
        self.am_name.delete(0, tk.END)
        self.am_price.delete(0, tk.END)
        self.am_time.delete(0, tk.END)
        messagebox.showinfo("Успех", f"Блюдо «{name}» добавлено")
    
    def admin_edit_dish(self):
        sel = self.menu_tree.selection()
        if not sel:
            messagebox.showwarning("Ошибка", "Выберите блюдо")
            return
        item = self.menu_tree.item(sel[0])
        did = item['values'][0]
        name = self.am_name.get().strip() or item['values'][1]
        try:
            price = float(self.am_price.get() or item['values'][2])
            time = int(self.am_time.get() or item['values'][4])
        except ValueError:
            messagebox.showerror("Ошибка", "Неверные числа")
            return
        cursor.execute("UPDATE menu SET name_of_dish=?, price=?, cooking_time=? WHERE id_dish=?",
                       (name, price, time, did))
        conn.commit()
        self.admin_refresh_menu()
        self.load_menu()
        messagebox.showinfo("Успех", "Блюдо обновлено")
    
    def admin_delete_dish(self):
        sel = self.menu_tree.selection()
        if not sel:
            messagebox.showwarning("Ошибка", "Выберите блюдо")
            return
        item = self.menu_tree.item(sel[0])
        did, name = item['values'][0], item['values'][1]
        if messagebox.askyesno("Удалить", f"Удалить «{name}»?"):
            cursor.execute("DELETE FROM menu WHERE id_dish=?", (did,))
            conn.commit()
            self.admin_refresh_menu()
            self.load_menu()
    
    def admin_toggle_dish(self):
        sel = self.menu_tree.selection()
        if not sel: return
        item = self.menu_tree.item(sel[0])
        did = item['values'][0]
        cursor.execute("UPDATE menu SET is_available = 1 - is_available WHERE id_dish=?", (did,))
        conn.commit()
        self.admin_refresh_menu()
        self.load_menu()
    
    def open_admin_composition(self):
        self.comp_win = tk.Toplevel(self.root)
        self.comp_win.title("🧪 Состав блюд")
        self.comp_win.geometry("700x450")
        
        tk.Label(self.comp_win, text="Выберите блюдо:", font=('Arial', 11)).pack(pady=10)
        
        top = tk.Frame(self.comp_win)
        top.pack()
        self.comp_dish = ttk.Combobox(top, state='readonly', width=40)
        self.comp_dish.pack(side='left', padx=5)
        cursor.execute("SELECT id_dish, name_of_dish FROM menu ORDER BY name_of_dish")
        self.comp_dishes = {name: did for did, name in cursor.fetchall()}
        self.comp_dish['values'] = list(self.comp_dishes.keys())
        if self.comp_dishes:
            self.comp_dish.set(list(self.comp_dishes.keys())[0])
        ttk.Button(top, text="📋 Показать состав", command=self.comp_refresh).pack(side='left', padx=5)
        
        # Таблица состава
        tree_f = tk.Frame(self.comp_win)
        tree_f.pack(fill='both', expand=True, padx=10, pady=10)
        sb = ttk.Scrollbar(tree_f)
        sb.pack(side='right', fill='y')
        self.comp_tree = ttk.Treeview(tree_f, columns=('ID', 'Продукт', 'Кол-во'), show='headings', height=10, yscrollcommand=sb.set)
        self.comp_tree.pack(fill='both', expand=True)
        sb.config(command=self.comp_tree.yview)
        self.comp_tree.heading('ID', text='ID')
        self.comp_tree.heading('Продукт', text='Продукт')
        self.comp_tree.heading('Кол-во', text='Кол-во на порцию')
        self.comp_tree.column('ID', width=50)
        self.comp_tree.column('Продукт', width=300)
        self.comp_tree.column('Кол-во', width=150, anchor='center')
        
        # Добавление ингредиента
        add_f = tk.Frame(self.comp_win)
        add_f.pack(fill='x', padx=10, pady=5)
        tk.Label(add_f, text="Продукт:").pack(side='left')
        self.comp_prod = ttk.Combobox(add_f, width=30)
        self.comp_prod.pack(side='left', padx=5)
        cursor.execute("SELECT id_product, name_of_product FROM products ORDER BY name_of_product")
        self.comp_products = {name: pid for pid, name in cursor.fetchall()}
        self.comp_prod['values'] = list(self.comp_products.keys())
        tk.Label(add_f, text="Кол-во:").pack(side='left')
        self.comp_qty = ttk.Entry(add_f, width=10)
        self.comp_qty.pack(side='left', padx=5)
        ttk.Button(add_f, text="➕ Добавить", command=self.comp_add).pack(side='left', padx=5)
    
    def comp_refresh(self):
        for item in self.comp_tree.get_children():
            self.comp_tree.delete(item)
        name = self.comp_dish.get()
        if name not in self.comp_dishes:
            return
        did = self.comp_dishes[name]
        cursor.execute('''
            SELECT dc.id, p.name_of_product, dc.quantity
            FROM dish_composition dc
            JOIN products p ON dc.id_product = p.id_product
            WHERE dc.id_dish = ?
        ''', (did,))
        for row in cursor.fetchall():
            self.comp_tree.insert('', 'end', values=row)
    
    def comp_add(self):
        name = self.comp_dish.get()
        prod = self.comp_prod.get()
        if name not in self.comp_dishes or prod not in self.comp_products:
            return
        try:
            qty = float(self.comp_qty.get())
        except ValueError:
            return
        did = self.comp_dishes[name]
        pid = self.comp_products[prod]
        cursor.execute("INSERT INTO dish_composition (id_dish, id_product, quantity) VALUES (?, ?, ?)", (did, pid, qty))
        conn.commit()
        self.comp_refresh()
    
    def open_admin_employees(self):
        self.emp_win = tk.Toplevel(self.root)
        self.emp_win.title("👤 Управление сотрудниками")
        self.emp_win.geometry("650x400")
        
        tk.Label(self.emp_win, text="Сотрудники", font=('Arial', 14, 'bold')).pack(pady=10)
        
        tree_f = tk.Frame(self.emp_win)
        tree_f.pack(fill='both', expand=True, padx=10)
        sb = ttk.Scrollbar(tree_f)
        sb.pack(side='right', fill='y')
        self.emp_tree = ttk.Treeview(tree_f, columns=('ID', 'Имя', 'Фамилия', 'Должность'), show='headings', height=10, yscrollcommand=sb.set)
        self.emp_tree.pack(fill='both', expand=True)
        sb.config(command=self.emp_tree.yview)
        self.emp_tree.heading('ID', text='ID')
        self.emp_tree.heading('Имя', text='Имя')
        self.emp_tree.heading('Фамилия', text='Фамилия')
        self.emp_tree.heading('Должность', text='Должность')
        self.emp_tree.column('ID', width=50, anchor='center')
        self.emp_tree.column('Имя', width=150)
        self.emp_tree.column('Фамилия', width=150)
        self.emp_tree.column('Должность', width=150)
        
        form = tk.Frame(self.emp_win)
        form.pack(fill='x', padx=10, pady=10)
        tk.Label(form, text="Имя:").grid(row=0, column=0)
        self.emp_name = ttk.Entry(form, width=15)
        self.emp_name.grid(row=0, column=1, padx=5)
        tk.Label(form, text="Фамилия:").grid(row=0, column=2)
        self.emp_surname = ttk.Entry(form, width=15)
        self.emp_surname.grid(row=0, column=3, padx=5)
        tk.Label(form, text="Должность:").grid(row=0, column=4)
        self.emp_pos = ttk.Combobox(form, width=15, state='readonly')
        self.emp_pos.grid(row=0, column=5, padx=5)
        cursor.execute("SELECT name FROM job_titles")
        self.emp_pos['values'] = [r[0] for r in cursor.fetchall()]
        
        btn_f = tk.Frame(self.emp_win)
        btn_f.pack(pady=5)
        ttk.Button(btn_f, text="➕ Добавить", command=self.emp_add).pack(side='left', padx=5)
        ttk.Button(btn_f, text="🗑️ Удалить", command=self.emp_delete).pack(side='left', padx=5)
        
        self.emp_refresh()
    
    def emp_refresh(self):
        for item in self.emp_tree.get_children():
            self.emp_tree.delete(item)
        cursor.execute('''
            SELECT e.id, e.name, e.surname, j.name
            FROM employees e JOIN job_titles j ON e.id_job_title = j.id ORDER BY e.id
        ''')
        for row in cursor.fetchall():
            self.emp_tree.insert('', 'end', values=row)
    
    def emp_add(self):
        name = self.emp_name.get().strip()
        surname = self.emp_surname.get().strip()
        pos = self.emp_pos.get()
        if not name or not surname or not pos:
            messagebox.showwarning("Ошибка", "Заполните все поля")
            return
        cursor.execute("SELECT id FROM job_titles WHERE name=?", (pos,))
        pid = cursor.fetchone()[0]
        cursor.execute("INSERT INTO employees (name, surname, id_job_title) VALUES (?, ?, ?)", (name, surname, pid))
        conn.commit()
        self.emp_refresh()
        self.load_waiters()
        self.emp_name.delete(0, tk.END)
        self.emp_surname.delete(0, tk.END)
    
    def emp_delete(self):
        sel = self.emp_tree.selection()
        if not sel: return
        item = self.emp_tree.item(sel[0])
        eid, name = item['values'][0], item['values'][1]
        if messagebox.askyesno("Удалить", f"Удалить сотрудника {name}?"):
            cursor.execute("DELETE FROM employees WHERE id=?", (eid,))
            conn.commit()
            self.emp_refresh()
            self.load_waiters()

    def show_staff_window(self):
        staff = tk.Toplevel(self.root)
        staff.title("👥 Персонал и зарплата")
        staff.geometry("850x550")
        staff.configure(bg='#fafafa')
        
        tk.Label(staff, text="Персонал и расчёт зарплаты", font=('Arial', 14, 'bold'), 
                 bg='#fafafa').pack(pady=10)
        
        # Выбор месяца
        f1 = tk.Frame(staff, bg='#fafafa')
        f1.pack(pady=5)
        tk.Label(f1, text="Месяц (ГГГГ-ММ):", bg='#fafafa', font=('Arial', 10)).pack(side='left')
        month_entry = ttk.Entry(f1, width=10, font=('Arial', 10))
        month_entry.pack(side='left', padx=10)
        month_entry.insert(0, datetime.now().strftime('%Y-%m'))
        
        # Таблица
        tree_frame = tk.Frame(staff, bg='#fafafa')
        tree_frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        scrollbar = ttk.Scrollbar(tree_frame)
        scrollbar.pack(side='right', fill='y')
        
        staff_tree = ttk.Treeview(tree_frame, 
                                   columns=('ФИО', 'Должность', 'Оклад', '%', 'База', 'Премия', 'Итого'),
                                   show='headings', height=12, yscrollcommand=scrollbar.set)
        staff_tree.pack(fill='both', expand=True)
        scrollbar.config(command=staff_tree.yview)
        
        staff_tree.heading('ФИО', text='ФИО')
        staff_tree.heading('Должность', text='Должность')
        staff_tree.heading('Оклад', text='Оклад (руб)')
        staff_tree.heading('%', text='%')
        staff_tree.heading('База', text='База для премии')
        staff_tree.heading('Премия', text='Премия (руб)')
        staff_tree.heading('Итого', text='Итого (руб)')
        
        staff_tree.column('ФИО', width=200)
        staff_tree.column('Должность', width=130)
        staff_tree.column('Оклад', width=100, anchor='center')
        staff_tree.column('%', width=50, anchor='center')
        staff_tree.column('База', width=150, anchor='center')
        staff_tree.column('Премия', width=120, anchor='center')
        staff_tree.column('Итого', width=120, anchor='center')
        
        total_label = tk.Label(staff, text="", font=('Arial', 11, 'bold'), bg='#fafafa', fg='#4a7c59')
        total_label.pack(pady=5)
        
        def refresh_staff():
            for item in staff_tree.get_children():
                staff_tree.delete(item)
            
            month_val = month_entry.get()
            
            # Считаем общую выручку кафе за месяц
            cursor.execute('''
                SELECT COALESCE(SUM(m.price * oi.quantity), 0)
                FROM orders o
                JOIN order_items oi ON o.id_order = oi.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE strftime('%Y-%m', o.created_at) = ?
                  AND o.status IN ('Оплачен', 'Выполнен')
            ''', (month_val,))
            total_revenue = cursor.fetchone()[0]
            
            # Считаем выручку бара (категория "Напитки")
            cursor.execute('''
                SELECT COALESCE(SUM(m.price * oi.quantity), 0)
                FROM orders o
                JOIN order_items oi ON o.id_order = oi.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                JOIN categories c ON m.id_category = c.id_category
                WHERE strftime('%Y-%m', o.created_at) = ?
                  AND o.status IN ('Оплачен', 'Выполнен')
                  AND c.name_category = 'Напитки'
            ''', (month_val,))
            bar_revenue = cursor.fetchone()[0]
            
            # Считаем выручку кухни (все категории кроме "Напитки")
            kitchen_revenue = total_revenue - bar_revenue
            
            total_payroll = 0
            
            cursor.execute('''
                SELECT e.id, e.name || ' ' || e.surname, j.name, j.salary_base, j.bonus_percent
                FROM employees e
                JOIN job_titles j ON e.id_job_title = j.id
                ORDER BY j.name, e.name
            ''')
            
            for emp_id, emp_name, pos, salary, bonus_pct in cursor.fetchall():
                salary = salary or 0
                bonus_pct = bonus_pct or 0
                
                if pos == 'Официант':
                    # Личная выручка официанта
                    cursor.execute('''
                        SELECT COALESCE(SUM(m.price * oi.quantity), 0)
                        FROM orders o
                        JOIN order_items oi ON o.id_order = oi.id_order
                        JOIN menu m ON oi.id_dish = m.id_dish
                        WHERE o.id_employee = ?
                          AND strftime('%Y-%m', o.created_at) = ?
                          AND o.status IN ('Оплачен', 'Выполнен')
                    ''', (emp_id, month_val))
                    base = cursor.fetchone()[0]
                    base_text = f"Личная: {base:.2f} ₽"
                    
                elif pos == 'Бариста' or pos == 'Бариста / Бармен':
                    # Выручка бара делится на всех бариста
                    cursor.execute("SELECT COUNT(*) FROM employees e JOIN job_titles j ON e.id_job_title = j.id WHERE j.name LIKE '%Бариста%' OR j.name LIKE '%Бармен%'")
                    bar_count = cursor.fetchone()[0] or 1
                    base = bar_revenue / bar_count
                    base_text = f"Бар: {bar_revenue:.2f} ÷ {bar_count}"
                    
                elif pos == 'Повар' or pos == 'Шеф-повар':
                    # Выручка кухни делится на всех поваров
                    cursor.execute("SELECT COUNT(*) FROM employees e JOIN job_titles j ON e.id_job_title = j.id WHERE j.name LIKE '%Повар%' OR j.name LIKE '%Шеф%'")
                    cook_count = cursor.fetchone()[0] or 1
                    base = kitchen_revenue / cook_count
                    base_text = f"Кухня: {kitchen_revenue:.2f} ÷ {cook_count}"
                    
                else:
                    # Администрация — от общей выручки
                    cursor.execute("SELECT COUNT(*) FROM employees e JOIN job_titles j ON e.id_job_title = j.id WHERE j.name IN ('Администратор', 'Администратор зала', 'Управляющий')")
                    admin_count = cursor.fetchone()[0] or 1
                    base = total_revenue / admin_count
                    base_text = f"Общая: {total_revenue:.2f} ÷ {admin_count}"
                
                bonus = base * bonus_pct / 100
                total = salary + bonus
                total_payroll += total
                
                staff_tree.insert('', 'end', 
                    values=(emp_name, pos, f"{salary:.2f}", f"{bonus_pct}%",
                            base_text, f"{bonus:.2f}", f"{total:.2f}"))
            
            total_label.config(text=f"Общий фонд оплаты труда за {month_val}: {total_payroll:.2f} ₽ | Выручка кафе: {total_revenue:.2f} ₽")
        
        ttk.Button(staff, text="🔄 Рассчитать", command=refresh_staff).pack(pady=5)
        refresh_staff()
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
        ttk.Button(windows_frame, text="📦 Продукты (ред.)", 
                   command=self.open_admin_products).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📝 Меню (ред.)", 
                   command=self.open_admin_menu).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="🧪 Состав блюд", 
                   command=self.open_admin_composition).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="👤 Сотрудники", 
                   command=self.open_admin_employees).pack(side='left', padx=5)

        ttk.Button(windows_frame, text="👨‍🍳 Экран кухни", 
                   command=self.show_kitchen_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📦 Склад продуктов", 
                   command=self.show_stock_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📊 Отчёты", 
                   command=self.show_reports_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="📋 Все заказы", 
                   command=self.show_orders_window).pack(side='left', padx=5)
        ttk.Button(windows_frame, text="👥 Персонал и ЗП", 
                    command=self.show_staff_window).pack(side='left', padx=5)
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
                WHERE DATE(o.created_at) = ? AND o.status IN ('Оплачен', 'Выполнен')
            ''', (date_val,))
            rev, checks = cursor.fetchone()
            
            text_revenue.insert(tk.END, f"=== ОТЧЁТ ПО ВЫРУЧКЕ ЗА {date_val} ===\n\n")
            text_revenue.insert(tk.END, f"Закрытых чеков: {checks or 0}\n")
            text_revenue.insert(tk.END, f"Общая выручка: {rev or 0:.2f} ₽\n\n")
            
            # Выручка по официантам
            text_revenue.insert(tk.END, "─" * 50 + "\n")
            text_revenue.insert(tk.END, "ВЫРУЧКА ПО ОФИЦИАНТАМ:\n\n")
            
            cursor.execute('''
                SELECT e.name || ' ' || e.surname, 
                       COUNT(DISTINCT o.id_order) as checks,
                       SUM(m.price * oi.quantity) as revenue
                FROM orders o
                JOIN employees e ON o.id_employee = e.id
                JOIN order_items oi ON o.id_order = oi.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE DATE(o.created_at) = ? AND o.status IN ('Оплачен', 'Выполнен')
                GROUP BY e.id
                ORDER BY revenue DESC
            ''', (date_val,))
            
            for name, ch, rev_sum in cursor.fetchall():
                text_revenue.insert(tk.END, f"  • {name}: {ch} чеков, {rev_sum:.2f} ₽\n")
            
            # Проданные блюда
            text_revenue.insert(tk.END, "\n" + "─" * 50 + "\n")
            text_revenue.insert(tk.END, "ПРОДАННЫЕ БЛЮДА:\n\n")
            
            cursor.execute('''
                SELECT m.name_of_dish, SUM(oi.quantity), SUM(m.price * oi.quantity)
                FROM order_items oi
                JOIN orders o ON oi.id_order = o.id_order
                JOIN menu m ON oi.id_dish = m.id_dish
                WHERE DATE(o.created_at) = ? AND o.status IN ('Оплачен', 'Выполнен')
                GROUP BY m.id_dish ORDER BY SUM(oi.quantity) DESC
            ''', (date_val,))
            
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
        orders_win.geometry("900x400")
        
        tree = ttk.Treeview(orders_win, 
                           columns=('ID', 'Стол', 'Официант', 'Дата', 'Статус'), 
                           show='headings', height=15)
        tree.pack(fill='both', expand=True, padx=10, pady=10)
        
        tree.heading('ID', text='№ Заказа')
        tree.heading('Стол', text='Стол')
        tree.heading('Официант', text='Официант')
        tree.heading('Дата', text='Дата и время')
        tree.heading('Статус', text='Статус')
        
        tree.column('ID', width=80, anchor='center')
        tree.column('Стол', width=60, anchor='center')
        tree.column('Официант', width=200)
        tree.column('Дата', width=180, anchor='center')
        tree.column('Статус', width=100, anchor='center')
        
        def refresh_orders():
            for item in tree.get_children():
                tree.delete(item)
            cursor.execute('''
                SELECT o.id_order, o.table_num, e.name || ' ' || e.surname, 
                       o.created_at, o.status 
                FROM orders o
                JOIN employees e ON o.id_employee = e.id
                ORDER BY o.id_order DESC LIMIT 50
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