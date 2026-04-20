-- 1. СОЗДАНИЕ ТАБЛИЦ

CREATE TABLE Positions (
    PositionID INT PRIMARY KEY AUTO_INCREMENT,
    Title VARCHAR(50) NOT NULL,
    Department VARCHAR(30),        -- Зал, Кухня, Администрация, Доставка
    AccessLevel INT DEFAULT 1,     -- 1: Видит меню/заказы, 5: Видит финансы
    SalaryBase DECIMAL(10, 2)
);

-- Таблица сотрудников
CREATE TABLE Employees (
    EmployeeID INT PRIMARY KEY AUTO_INCREMENT,
    FullName VARCHAR(100) NOT NULL,
    PositionID INT NOT NULL,
    Login VARCHAR(50) UNIQUE,
    PassHash VARCHAR(255), 
    IsActive BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (PositionID) REFERENCES Positions(PositionID)
);

-- Таблица продуктов (склад)
CREATE TABLE Products (
    ProductID INT PRIMARY KEY AUTO_INCREMENT,
    Name VARCHAR(100) NOT NULL,
    Unit VARCHAR(20) NOT NULL, -- кг, л, шт
    QuantityInStock DECIMAL(10, 3) DEFAULT 0,
    CostPerUnit DECIMAL(10, 2)
);

-- Таблица меню (блюда)
CREATE TABLE Menu (
    DishID INT PRIMARY KEY AUTO_INCREMENT,
    Name VARCHAR(100) NOT NULL,
    Category VARCHAR(50),
    Price DECIMAL(10, 2) NOT NULL,
    CookingTime INT, -- Время приготовления в минутах
    IsAvailable BOOLEAN DEFAULT TRUE
);

-- Таблица состава блюда (технологическая карта)
CREATE TABLE DishComposition (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    DishID INT NOT NULL,
    ProductID INT NOT NULL,
    Quantity DECIMAL(10, 3) NOT NULL, -- Сколько нужно продукта на 1 порцию
    FOREIGN KEY (DishID) REFERENCES Menu(DishID) ON DELETE CASCADE,
    FOREIGN KEY (ProductID) REFERENCES Products(ProductID)
);

-- Таблица заказов (чеки)
CREATE TABLE Orders (
    OrderID INT PRIMARY KEY AUTO_INCREMENT,
    TableNum INT NOT NULL,
    EmployeeID INT NOT NULL,
    OrderDate DATETIME DEFAULT CURRENT_TIMESTAMP,
    Status VARCHAR(20) DEFAULT 'Открыт', -- Открыт, Оплачен, Отменён
    FOREIGN KEY (EmployeeID) REFERENCES Employees(EmployeeID)
);

-- Таблица строк заказа (позиции в чеке)
CREATE TABLE OrderItems (
    ItemID INT PRIMARY KEY AUTO_INCREMENT,
    OrderID INT NOT NULL,
    DishID INT NOT NULL,
    Quantity INT DEFAULT 1,
    ReadyStatus BOOLEAN DEFAULT FALSE, -- Для кухни: Готово / Не готово
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID) ON DELETE CASCADE,
    FOREIGN KEY (DishID) REFERENCES Menu(DishID)
);

-- 2. НАПОЛНЕНИЕ ТЕСТОВЫМИ ДАННЫМИ (15-30 записей)

-- Должности
INSERT INTO Positions (Title, Department, AccessLevel, SalaryBase) VALUES
('Бариста / Бармен', 'Бар', 2, 30000.00),
('Официант', 'Зал', 1, 25000.00),
('Повар', 'Кухня', 2, 42000.00),
('Администратор зала', 'Администрация', 4, 45000.00),
('Управляющий', 'Администрация', 5, 70000.00),
('Шеф-повар', 'Кухня', 4, 65000.00),
('Посудомойщик / Уборщик', 'Кухня', 0, 22000.00),
('Курьер', 'Доставка', 1, 25000.00); -- Важно: если есть доставка, нужна таблица с адресами


-- Сотрудники
INSERT INTO Employees (FullName, PositionID, Login, PassHash, IsActive) VALUES
('Иванов Иван', 2, 'ivanov', '111', TRUE),       -- Официант
('Петрова Анна', 2, 'petrova', '111', TRUE),      -- Официант
('Сидоров Алексей', 1, 'bar', '222', TRUE),       -- Бариста
('Козлова Елена', 4, 'admin', '777', TRUE),       -- Админ зала
('Смирнов Павел', 3, 'cook1', '333', TRUE),       -- Повар
('Морозова Дарья', 3, 'cook2', '333', TRUE),      -- Повар
('Шеф Ивлев', 6, 'chef', '555', TRUE),            -- Шеф-повар (он же завхоз)
('Зайцева Марина', 7, 'clean', '000', TRUE),      -- Уборщица (может вообще без пароля)
('Быстров Олег', 8, 'courier', '888', TRUE),      -- Курьер
('Босс Молокосос', 5, 'boss', 'admin123', TRUE);  -- Управляющий

-- Продукты на складе (Набор типовых данных)
INSERT INTO Products (Name, Unit, QuantityInStock, CostPerUnit) VALUES
('Куриное филе', 'кг', 15.0, 320.00),
('Говядина мраморная', 'кг', 8.5, 1200.00),
('Картофель', 'кг', 45.0, 35.00),
('Помидоры черри', 'кг', 7.0, 250.00),
('Сыр пармезан', 'кг', 4.0, 1100.00),
('Масло оливковое', 'л', 5.0, 680.00),
('Мука пшеничная', 'кг', 20.0, 55.00),
('Кофе зерновой', 'кг', 3.0, 1400.00),
('Сахар', 'кг', 12.0, 60.00),
('Листья салата', 'кг', 3.5, 400.00);

-- Меню (Блюда)
INSERT INTO Menu (Name, Category, Price, CookingTime, IsAvailable) VALUES
('Цезарь с курицей', 'Салаты', 450.00, 15, TRUE),
('Стейк Рибай', 'Горячее', 1250.00, 25, TRUE),
('Картофель фри', 'Гарниры', 180.00, 10, TRUE),
('Паста Карбонара', 'Паста', 520.00, 20, TRUE),
('Чизкейк Нью-Йорк', 'Десерты', 320.00, 5, TRUE),
('Американо', 'Напитки', 150.00, 5, TRUE),
('Лимонад домашний', 'Напитки', 180.00, 7, TRUE);

-- Состав блюд (Техкарты)
INSERT INTO DishComposition (DishID, ProductID, Quantity) VALUES
(1, 1, 0.150), -- Цезарь: 150г курицы
(1, 4, 0.050), -- Цезарь: 50г черри
(1, 5, 0.030), -- Цезарь: 30г пармезана
(1, 10, 0.050), -- Цезарь: 50г салата
(2, 2, 0.300), -- Стейк: 300г говядины
(3, 3, 0.200), -- Картофель фри: 200г картофеля
(6, 8, 0.018); -- Американо: 18г кофе на порцию

-- Заказы (Примеры чеков)
INSERT INTO Orders (TableNum, EmployeeID, OrderDate, Status) VALUES
(1, 1, '2026-04-20 12:30:00', 'Оплачен'),
(3, 2, '2026-04-20 13:15:00', 'Открыт'),
(2, 1, '2026-04-20 14:00:00', 'Оплачен');

-- Строки заказов (Что заказали)
INSERT INTO OrderItems (OrderID, DishID, Quantity, ReadyStatus) VALUES
-- Заказ 1 (Стол 1)
(1, 1, 2, TRUE), -- 2 Цезаря
(1, 2, 1, TRUE), -- 1 Стейк
(1, 6, 2, TRUE), -- 2 Американо
-- Заказ 2 (Стол 3) - Текущий открытый заказ
(2, 4, 1, FALSE), -- 1 Карбонара (готовится)
(2, 7, 2, FALSE), -- 2 Лимонада (готовятся)
-- Заказ 3 (Стол 2)
(3, 5, 2, TRUE), -- 2 Чизкейка
(3, 3, 1, TRUE); -- 1 Фри