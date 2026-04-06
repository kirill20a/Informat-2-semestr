import sqlite3

def run_query(query, description):
    """Выполняет запрос и красиво выводит результат"""
    conn = sqlite3.connect("baza.db")
    cursor = conn.cursor()
    
    print(f"\n📌 {description}")
    print("-" * 40)
    
    cursor.execute(query)
    results = cursor.fetchall()
    
    for row in results:
        print(row)
    
    conn.close()

# ===== ПРОСТЫЕ ЗАПРОСЫ =====
run_query("SELECT COUNT(*) FROM employees", "Общее количество сотрудников")
run_query("SELECT COUNT(*) FROM job_titles", "Общее количество должностей")
run_query("SELECT MAX(id) FROM employees", "Максимальный ID сотрудника")
run_query("SELECT SUM(id_job_title) FROM job_titles", "Сумма ID должностей")
run_query("SELECT AVG(id) FROM employees", "Средний ID сотрудников")

# ===== ЗАПРОСЫ С АГРЕГАЦИЕЙ =====
run_query("""
    SELECT jt.name, COUNT(e.id) 
    FROM job_titles jt 
    LEFT JOIN employees e ON jt.id_job_title = e.id_job_title 
    GROUP BY jt.name
""", "Количество сотрудников по должностям")

# ===== ЗАПРОСЫ С ОБЪЕДИНЕНИЕМ =====
run_query("""
    SELECT e.surname, e.name, jt.name 
    FROM employees e 
    JOIN job_titles jt ON e.id_job_title = jt.id_job_title
""", "Сотрудники с названиями должностей")