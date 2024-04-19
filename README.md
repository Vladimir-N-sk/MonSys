# MonSys
Этот проект достался мне много лет назад в качестве поддержки и сопровождения.
Это проект по мониторингу инженерных систем (в основном систем энергетики) поэтому я назвл его 
Monitoring System (сокращенно MonSys). 

Основная функция проекта сбор информации с оборудования и запись в БД. В качестве БД выбрана PostgreSQL.
Для создания необходимых таблиц БД в проекте есть файл monsys_create_db.

Добавлена возможность выгрузки собранной информации с оборудования в файл JSON-формат для поддержки 
сопряжения с zabbix-agent через UserParameter.

Поддерживаемые протоколы опроса оборудования:

1.Общераспространённые
-SNMP (с помошью внешней библиотеки NET-SNMP);
-ModBus RTU, ModBus-TCP;

2.Корпоративные
-ICP DAS;
-Счетчики электроэнергии СЕ301-303, СЕ102;
-Контроллеры ДГУ BE2000 Bernini
-ИБП Эко-Мастер
-ЭПУ Eltek MiniFlex, AEON, Flatpack


Вся настройка работы ведется через файл monsys.cfg

Сборка проекта ведется через CMakeLists.txt команндой
/usr/bin/cmake --build /home/<домашняя дирректория>/<файлы проекта>/build --config Release --target all -- -j 4
пример
/usr/bin/cmake --build /home/monsys/MonSys/build --config Release --target all -- -j 4