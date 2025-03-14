# Анализ бинарного кода программ

## Введение

### Задание

Задание состояло из двух частей: взлом программы с помощью переполнения буфера и написания "патчера"

### Используются:

- Дизассемблер "IDA"
- Отладчик "TurboDebugger"

## Анализ кода

### Разбор диззасемблированного кода

IDA показал, что в файле содержится всего 7 функций. После просмотра их в режиме графов стало понятно, что несколько из них сделаны специально чтобы запутать человека с дизассемблером. В тектовом формате они выглядят достаточно большими, однако всё что они делают это показывают на экране отладчика непонятный набор команд. Все эти команды пропускаются с помощью команды jmp.
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/1.1.png)
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/1.2.png)
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/1.3.png)

После запуска отладчика было замечено, что это на самом деле не команды, а строки, содержищие призывы к непонятным действиям.
Сразу отсеиваем эти функции, чтобы в дальнейшем анализе не отвлекаться на них.
Идём далее по потоку выполнения
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/2.png)

В функции sub_F4 вызов упомянутых ранее функций не дал дизассемблеру разобрать прерывания DOS'а. Однако сочетания 'mov ah, 9' и 'mov ah, 3fh' дали понять, что здесь есть вывод на экран и принятие какого-то текста от пользоватеся. После запуска программы стало понятно что первые действия такого плана это как раз ввод пароля, который нас интересует. Также с помощью дизасемблера была найдена уязвимость: программист практически не отслеживает длину пароля, однако он не может достигнуть стека, что не даст поменять адрес возврата.
Переименуем эту функцию в дизассемблере в 'get_password'
Адрес буфера пароля: 348h(с учётом отступа в 256 байт). Перейдя к адресу 348h - 100h = 248h в IDA можно увидеть что пароль лежит в конце памяти программы. Значит не удастся поменять программу при вводе пароля пароль.
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/3.png)

Следующей идёт вызов функции sub_45. Она по логике должна проверять правильность вводимого пароля. Однако в ней нет ничего похожего на эту проверку. Запускаем программу и видим что после ввода пароля(любого) нас спрашивают: "Are you ready for pizdec?" убедившись с совпадением выводимой строки стало понятно что эта функция разделяет поток на "pizdec" и "ne pizdec". Переименуем функцию в дизассемблере в 'ask_pizdec'
Перейдём на адрес 341h - 100h = 241h в IDA. Можно заметить что для буфера ответа выделено всего 3 байта, однако длина введённой строки может достигать 7 символов.
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/4.png)

При согласии с "pizdec" происходит вызов функции sub_D1. В ней находится цикл, который кладёт в стек достаточно много чисел. Однако далее идёт возврат, который не может пройти правильно. Оказывается программист предусмотрел возможность пользователю перезагрузить компьютер.
Переименуем эту функцию в дизассемблере в "pizdec".
Вспомнив про уязвимость с длиной вводимого пароля была найдена уязвимость в этой функции: после её выполнения поток выполнения попадает на адрес 1488h. Так как длина пароля может достигнуть этого адреса, туда можно записать нужный нам код, который поменяет правильный пароль на нужный нам. Осталось только найти, где в памяти лежит правильный пароль.
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/5.png)

Далее попадаем в достаточно большую функцию sub_168. В ней есть что-то похожее на сравнение паролей. Сравниваются 4 байта на уже известном нам как буфер пароля адресе 348h и пока безызвестном нам адресе 344h. Просмотрев этот адрес в текстовом режиме стало понятно что это адрес правильного пароля.
![alt text](https://raw.githubusercontent.com/artemneskorodov/crack/2877ffe61b6aef7bfa6fb2b15a7d2f2483a4ba22/6.png)

Вспоминая найденые ранее ошибки проверок длины вводимого пароля и ответа на вопрос очевидными становятся 2 возможных подхода для взлом программы:

#### 1. Ввести любой пароль длиной 4 символа (например 1234) при вводе пароля и затем любых три символа и тот же пароль в конце при ответе на вопрос (например ххх1234).
#### 2. Создать файл с паролем, в котором лежит сначала 1488h - 348h любых символов, а затем двоичное представление функции, которая по адресу 344h меняет проверяемый пароль на нужный нам и уберёт из стека в функции "pizdec" лишние элементы.

Для выполнения второго пункта была написана функция на ассемблере. Она была скомпилирована и затем в её двоичном коде все адреса (кроме соответственно 348h) были заменены по правилу: x --> (x + 1448h - 348h).

### Написание патчера

Патчер меняет пароль в программе на известный. Пароль сделан константным, однако легко добавить возможность вводить пароль вручную. Чтобы программа изменила состовляющее бинарного файла нужно выиграть у неё в небольшую игру, написание которой заняло совсем немного времени.
