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

-- Таблица категорий товаров
CREATE TABLE IF NOT EXISTS categories (
    id_category INTEGER PRIMARY KEY AUTOINCREMENT,
    name_category TEXT NOT NULL
);

-- Таблица товаров
CREATE TABLE IF NOT EXISTS products (
    id_product INTEGER PRIMARY KEY AUTOINCREMENT,
    name_of_product TEXT NOT NULL,
    price REAL NOT NULL,
    id_category INTEGER NOT NULL,
    quantity_at_storage REAL NOT NULL,
    FOREIGN KEY (id_category) REFERENCES categories(id_category)
);

-- Таблица чеков
CREATE TABLE IF NOT EXISTS receipts (
    id_check INTEGER PRIMARY KEY AUTOINCREMENT,
    created_at TEXT NOT NULL,
    id_cashier INTEGER NOT NULL,
    FOREIGN KEY (id_cashier) REFERENCES employees(id)
);

-- Таблица позиций в чеке
CREATE TABLE IF NOT EXISTS sale_items (
    id_sale INTEGER PRIMARY KEY AUTOINCREMENT,
    id_check INTEGER NOT NULL,
    id_product INTEGER NOT NULL,
    quantity REAL NOT NULL,
    FOREIGN KEY (id_check) REFERENCES receipts(id_check),
    FOREIGN KEY (id_product) REFERENCES products(id_product)
);