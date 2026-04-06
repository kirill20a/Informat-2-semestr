import os
import subprocess
import sys

def run_script(script_name, description):
    """Запускает Python скрипт и выводит результат"""
    print(f"\n{'='*60}")
    print(f" ЗАПУСК: {description}")
    print(f"{'='*60}")
    
    result = subprocess.run([sys.executable, script_name], capture_output=True, text=True)
    print(result.stdout)
    if result.stderr:
        print("ОШИБКИ:")
        print(result.stderr)

if __name__ == "__main__":
    print("🐍 ЛАБОРАТОРНАЯ РАБОТА: SQLite в Python")
    print("="*60)
    
    # Шаг 1: Создание таблиц
    run_script("db_creator.py", "Создание и заполнение таблиц")
    
    # Шаг 2: Выполнение запросов
    run_script("db_queries.py", "Выполнение SQL-запросов")
    
    # Шаг 3: Импорт данных
    run_script("import_data.py", "Импорт из внешних файлов")
    
    print("\n✅ ВСЕ ЗАДАНИЯ ВЫПОЛНЕНЫ УСПЕШНО!")