import sqlite3

# 1. Подключаемся к базе данных (файл создастся автоматически)
connection = sqlite3.connect("baza.db")
cursor = connection.cursor()

# 2. Создаем таблицу должностей (сначала ту, на которую ссылаются)
cursor.execute("""
CREATE TABLE IF NOT EXISTS job_titles (
    id_job_title INTEGER PRIMARY KEY NOT NULL UNIQUE,
    name TEXT NOT NULL
);
""")

# 3. Создаем таблицу сотрудников с внешним ключом
cursor.execute("""
CREATE TABLE IF NOT EXISTS employees (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    surname TEXT NOT NULL,
    name TEXT NOT NULL,
    id_job_title INTEGER NOT NULL,
    FOREIGN KEY (id_job_title) REFERENCES job_titles(id_job_title)
);
""")

print("✅ Таблицы успешно созданы")

# 4. Заполняем таблицу должностей
job_titles_data = [
    (1, 'Менеджер'),
    (2, 'Разработчик'),
    (3, 'Аналитик'),
    (4, 'Дизайнер')
]
cursor.executemany(
    "INSERT OR IGNORE INTO job_titles (id_job_title, name) VALUES (?, ?)", 
    job_titles_data
)
print(f"✅ Добавлено должностей: {len(job_titles_data)}")

# 5. Заполняем таблицу сотрудников
employees_data = [
    (1, 'Иванов', 'Иван', 2),
    (2, 'Петров', 'Петр', 1),
    (3, 'Сидорова', 'Мария', 3),
    (4, 'Козлов', 'Алексей', 2),
    (5, 'Васильева', 'Ольга', 4)
]
cursor.executemany(
    "INSERT OR IGNORE INTO employees (id, surname, name, id_job_title) VALUES (?, ?, ?, ?)", 
    employees_data
)
print(f"✅ Добавлено сотрудников: {len(employees_data)}")

# 6. Сохраняем изменения
connection.commit()
connection.close()
print("💾 Изменения сохранены. База данных 'baza.db' готова!")