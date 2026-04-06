import sqlite3
#1 Подключение к базе данных
conn=sqlite3.connect('student.db')
cursor=conn.cursor()

#2 Создание Таблиц

cursor.execute('''
    CREATE TABLE IF NOT EXISTS Направления (
        id INTEGER PRIMARY KEY,
        название TEXT
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS Формы_обучения (
        id INTEGER PRIMARY KEY,
        название TEXT
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS Уровни_образования (
        id INTEGER PRIMARY KEY,
        название TEXT
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS Студенты (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        фамилия TEXT,
        имя TEXT,
        отчество TEXT,
        id_направления INTEGER,
        id_формы INTEGER,
        id_уровня INTEGER,
        средний_балл REAL,
        FOREIGN KEY (id_направления) REFERENCES Направления(id),
        FOREIGN KEY (id_формы) REFERENCES Формы_обучения(id),
        FOREIGN KEY (id_уровня) REFERENCES Уровни_образования(id)
    )
''')

#3 Заполнение Данными
# Очищаем таблицы перед заполнением (чтобы при повторном запуске не было дублей)
cursor.execute('DELETE FROM Студенты')
cursor.execute('DELETE FROM Направления')
cursor.execute('DELETE FROM Формы_обучения')
cursor.execute('DELETE FROM Уровни_образования')

#Заполняем справочники
cursor.executemany('INSERT INTO Направления VALUES (?,?)',[
    (1,'Прикладная Информатика'),
    (2,'Программная Инженерия'),
    (3,'Бизнес-Информатика')
])

cursor.executemany('INSERT INTO Формы_обучения VALUES (?, ?)', [
    (1, 'очная'),
    (2, 'вечерняя'),
    (3, 'заочная')
])

cursor.executemany('INSERT INTO Уровни_образования VALUES (?, ?)', [
    (1, 'бакалавр'),
    (2, 'магистр')
])

# Заполняем Студентами
cursor.executemany('''
    INSERT INTO Студенты (фамилия, имя, отчество, id_направления, id_формы, id_уровня, средний_балл)
    VALUES (?, ?, ?, ?, ?, ?, ?)
''', [
    ('Иванов', 'Иван', 'Иванович', 1, 1, 1, 4.8),
    ('Петров', 'Петр', 'Петрович', 1, 1, 1, 4.9),
    ('Сидоров', 'Алексей', 'Сергеевич', 1, 1, 1, 4.7),
    ('Кузнецова', 'Анна', 'Игоревна', 1, 1, 1, 4.9),
    ('Кузнецов', 'Дмитрий', 'Алексеевич', 1, 1, 1, 4.6),
    ('Васильев', 'Иван', 'Петрович', 2, 2, 1, 4.2),
    ('Васильева', 'Елена', 'Владимировна', 2, 1, 2, 4.5),
    ('Новиков', 'Антон', 'Сергеевич', 3, 3, 2, 3.9)
])

conn.commit()
# ==================================================
# 5. Закрытие Соединения

conn.close()
print("\n✅ Готово! База данных сохранена в файл 'students.db'")