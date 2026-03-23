-- =====================================================
-- ЗАДАНИЕ 3 ЕГЭ ПО ИНФОРМАТИКЕ
-- СОВМЕЩЕННЫЙ ЗАПРОС: детали операций + итоговый ответ
-- =====================================================

-- =====================================================
-- ЧАСТЬ 1: Детальные строки
-- =====================================================

SELECT 
    m.date,
    m.shop_id,
    s.district,
    p.product_name,
    m.quantity,
    m.operation_type,
    CASE 
        WHEN m.operation_type = 'Поступление' THEN m.quantity
        ELSE -m.quantity
    END AS change
FROM movements m
JOIN shops s ON m.shop_id = s.shop_id
JOIN products p ON m.product_id = p.product_id
WHERE m.product_id = 15
    AND m.date BETWEEN '2021-06-01' AND '2021-06-10'
    AND s.district = 'Заречный'
ORDER BY m.date, m.shop_id, m.operation_type;

-- =====================================================
-- ЧАСТЬ 2: Итоговый ответ
-- =====================================================

SELECT 
    'ИТОГО:' AS description,
    SUM(CASE WHEN operation_type = 'Поступление' THEN quantity ELSE 0 END) AS total_in,
    SUM(CASE WHEN operation_type = 'Продажа' THEN quantity ELSE 0 END) AS total_out,
    SUM(CASE WHEN operation_type = 'Поступление' THEN quantity ELSE -quantity END) AS total_change
FROM movements m
WHERE m.product_id = 15
    AND m.date BETWEEN '2021-06-01' AND '2021-06-10'
    AND m.shop_id IN (
        SELECT shop_id 
        FROM shops 
        WHERE district = 'Заречный'
    );