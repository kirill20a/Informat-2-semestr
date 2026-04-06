import sqlite3
import csv

def import_from_csv():
    """Импорт данных из CSV файла"""
    connection = sqlite3.connect("baza.db")
    cursor = connection.cursor()
    
    try:
        with open('new_jobs.csv', 'r', encoding='utf-8') as file:
            # Читаем CSV с заголовком
            reader = csv.DictReader(file)
            for row in reader:
                cursor.execute("""
                    INSERT OR IGNORE INTO job_titles (id_job_title, name) 
                    VALUES (?, ?)
                """, (row['id_job_title'], row['name']))
        
        print("✅ Данные из new_jobs.csv импортированы")
    except FileNotFoundError:
        print("❌ Файл new_jobs.csv не найден")
    except Exception as e:
        print(f"❌ Ошибка при импорте CSV: {e}")
    
    connection.commit()
    connection.close()

def import_from_txt():
    """Импорт данных из TXT файла"""
    connection = sqlite3.connect("baza.db")
    cursor = connection.cursor()
    
    try:
        with open('new_employees.txt', 'r', encoding='utf-8') as file:
            # Пропускаем заголовок
            next(file)
            for line in file:
                # Разделяем по точке с запятой
                data = line.strip().split(';')
                if len(data) == 4:
                    cursor.execute("""
                        INSERT OR IGNORE INTO employees (id, surname, name, id_job_title) 
                        VALUES (?, ?, ?, ?)
                    """, data)
        
        print("✅ Данные из new_employees.txt импортированы")
    except FileNotFoundError:
        print("❌ Файл new_employees.txt не найден")
    except Exception as e:
        print(f"❌ Ошибка при импорте TXT: {e}")
    
    connection.commit()
    connection.close()

def check_import_result():
    """Проверка результатов импорта"""
    connection = sqlite3.connect("baza.db")
    cursor = connection.cursor()
    
    print("\n📊 ИТОГОВЫЕ ДАННЫЕ ПОСЛЕ ИМПОРТА")
    print("-" * 40)
    
    cursor.execute("SELECT * FROM job_titles")
    print("Должности:")
    for row in cursor.fetchall():
        print(f"   {row}")
    
    cursor.execute("SELECT * FROM employees")
    print("\nСотрудники:")
    for row in cursor.fetchall():
        print(f"   {row}")
    
    connection.close()

if __name__ == "__main__":
    print("🚀 Начинаем импорт данных...")
    import_from_csv()
    import_from_txt()
    check_import_result()