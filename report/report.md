# Лабораторная работа по курсу "Параллельное программирование"
# Решение волнового уравнения

Выполнил студент Владимир Сеннов, гр. 3540201/20101.

## Постановка задачи

В рамках работы необходимо выполнить следующие задания.
1. Разработать алгоритм решения волнового уравнения, позволяющий 
использовать параллельные вычисления.
2. Разработать набор тестов с использованием математических пакетов.
3. Реализовать алгоритмы при помощи следующих технологий:
    * Язык C + Linux pthreads,
    * Язык C + MPI,
    * Язык Python + MPI,
    * Язык C + OpenMP.
4. Провести исследование эффекта от использования многоядерности / 
многопоточности / многопроцессорности на СКЦ, варьируя количество процессов
/ потоков и количество узлов от 1 до 4 для технологии MPI.

## Математическое описание
### Волновое уравнение

В общем случае волновое уравнение имеет следующий вид:

![Волновое уравнение](https://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%5E2%20u%7D%7B%5Cpartial%20t%5E2%7D%20%3D%20%5Cfrac%7B1%7D%7Bv%5E2%7D%5Ccdot%5CDelta%20u%20&plus;%20f), 

где _u_ — волновая функция, _f_ — внешнее воздействие, _v_ — фазовая
скорость.

В рамках данной работы будет решено одномерное однородное волновое уравнение
с краевыми условиями. Волновое уравнение выглядит следующим образом:

![Одномерное однородное уравнение](https://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%5E2%20u%7D%7B%5Cpartial%20t%5E2%7D%20%3D%20a%5E2%5Cfrac%7B%5Cpartial%5E2%20u%7D%7B%5Cpartial%20x%5E2%7D),  (1)

где _u(x, t)_ — волновая функция, _a = 1/v_. Граничные условия заданы так:
* _0 ≤ t ≤ T_, 
* _0 ≤ x ≤ l_,
* ![](https://latex.codecogs.com/gif.latex?u%28x%2C%200%29%20%3D%20%5Cphi_1%28x%29),
* ![](https://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%20u%7D%7B%5Cpartial%20t%7D%28x%2C%200%29%20%3D%20%5Cphi_2%28x%29),
* ![](https://latex.codecogs.com/gif.latex?u%280%2C%20t%29%20%3D%20%5Cpsi_1%28t%29),
* ![](https://latex.codecogs.com/gif.latex?u%28l%2C%20t%29%20%3D%20%5Cpsi_2%28t%29).


### Разностная схема
Промежуток _[0; T]_ разбиваем на _P_ частей с шагом _τ = T / P_, промежуток 
_[0; l]_ разбиваем на _M_ частей с шагом _h = l / M_. Тогда уравнение (1)
заменим разностной схемой:

![](https://latex.codecogs.com/gif.latex?%5Cfrac%7Bu%5E%7Bp&plus;1%7D_m%20-%202u%5Ep_m&plus;u%5E%7Bp-1%7D_m%7D%7B%5Ctau%5E2%7D%20%3D%20a%5E2%5Cfrac%7Bu%5Ep_%7Bm&plus;1%7D-2u%5Ep_m&plus;u%5Ep_%7Bm-1%7D%7D%7Bh%5E2%7D), 

где ![](https://latex.codecogs.com/gif.latex?u%5Ep_m%20%3D%20u%28m%5Ccdot%20h%2C%20p%20%5Ccdot%5Ctau%29). 
Отсюда получаем расчетную формулу: 

![](https://latex.codecogs.com/gif.latex?u%5E%7Bp&plus;1%7D_m%20%3D%202u%5Ep_m%20-%20u%5E%7Bp-1%7D_m%20&plus;%20%5Cfrac%7Ba%5E2%20%5Ctau%5E2%7D%7Bh%5E2%7D%5Cleft%28u%5Ep_%7Bm-1%7D-2u%5Ep_m&plus;u%5Ep_%7Bm-1%7D%5Cright%29). (2)

Из граничных условий получаем следующее: 
* ![](https://latex.codecogs.com/gif.latex?u%5E0_m%20%3D%20%5Cphi_1%28m%5Ccdot%20h%29), 
* ![](https://latex.codecogs.com/svg.image?u^1_m&space;=&space;\phi_1(h\cdot&space;m)&plus;\tau\cdot\phi_2(h\cdot&space;m)&plus;\frac{\tau^2}{2}\left(a^2\cdot\frac{\phi_1(h\cdot(m&plus;1))-2\phi_1(h\cdot&space;m)&plus;\phi_1(h\cdot(m-1))}{h^2}\right)), (3) 
* ![](https://latex.codecogs.com/gif.latex?u%5Ep_0%20%3D%20%5Cpsi_1%28p%5Ccdot%20%5Ctau%29%2C%20u_M%5Ep%20%3D%20%5Cpsi%28p%5Ccdot%20%5Ctau%29). 

Точность расчета по формуле (2) составляет ![](https://latex.codecogs.com/gif.latex?O%28%5Ctau%5E3%20&plus;%20h%5E3%29).

### Последовательный алгоритм расчета
**Вход:**
* _P_ — количество шагов по времени, _M_ — количество шагов по координате 
_x_;
* _τ_ — длина шага по времени, _h_ — длина шага по координате _x_, _a_ — 
величина, обратная фазовой скорости;
* _ϕ1_, _ϕ2_ — `array[1..M] of real`, начальные условия, значение _u_ и 
_du/dt_ соответственно;
* _ψ1_, _ψ2_ — `array[1..P] of real`, граничные условия, левый край и правый
соответственно.

**Выход:**
* _u_ — `array[1..P][1..M] of real`, решение волнового уравнения.

**Ограничения:**
* _|τa / h| < 1_ — условие сходимости разностной схемы.

**Алгоритм:**
1. Заполнить значения `u[1][x] = 𝜙1[x]` для `x` от 1 до _M_.
2. Заполнить значения `u[2][x]` для `x` от 1 до _M_ по формуле (3).
3. Заполнить значения `u[t][1] = ψ1[t]`, `u[t][M] = ψ2[t]` для `t` от 3 до 
_P_. 
4. Для каждого `p` от 3 до _P_ вычислить значения `u[p][m]`  для `m` от 
2 до _M-1_ по формуле (2).

### Параллельный алгоритм с общей памятью
Вычисления можно распараллелить по `m` на шаге 4 последовательного 
алгоритма. Каждый поток имеет доступ к массиву `u` и пишет для собственных 
значений `m`. Когда все потоки закончат расчет, необходимо перейти к 
следующему `p`. Так же можно распараллелить и операции на шагах 1, 2, 3, 
поскольку они тоже являются векторными.

### Параллельный алгоритм с распределенной памятью
Данные можно легко распределить на несколько узлов, предоставив каждому 
узлу хранить `u` для `x` в некотором промежутке. Легко видеть, что на каждом
промежутке необходимо решить такое же волновое уравнение, но с другими 
граничными условиями. Граничными условиями являются значения волновой 
функции на краях промежутка из соседних промежутков. Узлам необходимо 
обменяться этими значениями для перехода к следующему значению `p`. 

Каждый узел рассчитывает значения волновой функции согласно параллельному 
алгоритму с общей памятью.

Изначально исходные данные находятся у главного процесса и должны быть 
распределены между всем остальными узлами. После всех вычислений данные 
необходимо собрать обратно у главного процесса. Сделать это можно по принципу 
"разделяй и властвуй". Ниже представлен алгоритм распределения данных для 
массива длиной _M_ между _N_ процессами, причем последовательность данных 
сохраняется для рангов процессов, то есть для массива _[1, 2, 3, 4, 5, 6]_ 
процесс 0 получит _[1, 2]_ , процесс 1 получит _[3, 4]_ , процесс 2 получит 
_[5, 6]_ .

**Вход:**
* _N_ — количество процессов;
* _A_ — `array[1..M] of any` массив, который нужно распределить между процессами.

**Ограничения:**
* _N <= M_.

**Алгоритм:**
1. _B_ — локальный буфер каждого процесса, _S_ — размер этого буфера; у процесса 
0 _B <- A_, _S <- M_, остальные процессы подготавливают этот буфер перед 
получением данных.
2. _k <- max{ 2^k | 2^k < N }_ — шаг отправки; _Ps <- [M / N]_ — количество 
данных, которое получат все процессы кроме нулевого.
3. _i <- 0_.
4. _ts <- min{N - (i + k), k} * Ps_ ; процесс _i_ отправляет данные 
_B[(S - ts + 1) .. S]_ процессу _i + k_ и устанавливает _S <- S - ts_ ;
процесс _(i + k)_ принимает _ts_ данных от процесса _i_ в _B_ и устанавливает 
_S <- ts_; если _(i + 2k) < N - k_, то _i <- i + 2k_ и начинаем шаг снова, иначе 
переходим к шагу 5.
5. Если _k > 1_, то _k <- k/2_ и переходим к шагу 4, иначе завершение алгоритма. 

Алгоритм сбора данных абсолютно аналогичен, только нужно поменять местами 
отправку и получение. В алгоритме распределения отправлялся конец буфера, а
получатель писал в начало буфера, в алгоритме сбора получатель пишет в конец 
своего буфера, а отправляет буфер целиком.

## Тестирование 
Для тестирования используем волновое уравнение, решение которого можно найти 
аналитически. 

Пускай _𝜙1 = ψ1 = ψ2 = 0_, a 

![](https://latex.codecogs.com/svg.image?\phi_2&space;=&space;\frac{\pi&space;a}{l}\sin\frac{\pi&space;x}{l}).

Тогда решение должно быть близко к стоячей волне: 

![](https://latex.codecogs.com/svg.image?u(x,&space;t)&space;=&space;\sin\frac{\pi&space;x}{l}\sin\frac{\pi&space;at}{l}).

Тестирование алгоритма проводится со следующими параметрами:
* _M = P = 500_,
* _h = τ = 0.1_,
* _a = 0.5_.

Точность вычисления на каждом этапе имеет порядок ![](https://latex.codecogs.com/gif.latex?O%28%5Ctau%5E3%20&plus;%20h%5E3%29)
, так что приемлимым будем считать максимальное отклонение 
волновой функции по всей сетке не больше _5e-3_.


## Исследование скорости работы
Для исследования скорости работы решается уравнение со следующими параметрами:
* _𝜙1 = 𝜙2 = ψ2 = 0_, _ψ1 = sin(6πt/T)_;
* _h = τ = 0.1_,
* _a = 0.5_;
* параметры _M_ и _P_ настраиваются для каждого запуска.

Эксперимент зависит от следующих параметров:
* _M_, _P_ — размерность задачи;
* _N_ — количество потоков.

Параллельный алгоритм будет работать неэффективно при _N > M_, для более 
наглядного исследования эффекта от многопоточности необходимо иметь _N << M_.
Количество однотипных итераций напрямую зависит от _P_, поэтому _P_ можно 
в эксперименте не варьировать, а взять достаточно большим, чтобы наблюдать 
за тем, как реализация отдельной итерации влияет на результат в целом.

Максимальное количество эффективных потоков зависит от количества ядер 
процессора, но никто не мешает сделать больше потоков, даже если они будут 
работать неэффективно. 

### Исследование на одном узле

Целевая платформа — узел суперкомпьютера, на каждом узле которого есть два 
процессора с 28 реальными ядрами и 56 виртуальными ядрами.

Эксперимент проводится со следующими параметрами:
* _N : {1, 2, 4, 14, 28, 56}_ ;
* _P =  5000_ ;
* _M : {4000, 5000, 10000, 20000, 50000}_
* каждое измерение повторяется 20 раз.

Результат каждого измерения выводится в формате .csv со следующими колонками:
* **id** — номер измерения;
* **N** — количество потоков;
* **M** — величина _M_ ;
* **t** — время работы в секундах.

### Исследование на нескольких узлах

Для технологии MPI проведем два эксперимента: на 2 узлах и на 5 узлах.

Эксперимент с 2 узлами проводится со следующими параметрами:
* _N : {2, 4, 14, 28, 56, 84, 112}_;
* _P = 5000_, _M = 50000_;
* каждое измерение повторяется 5 раз.

Эксперимент с 5 узлами проводится со следующими параметрами:
* _N : {28, 56, 84, 112, 140, 168, 224, 280}_;
* _P = 5000_, _M = 50000_;
* каждое измерение повторяется 5 раз.

## Особенности реализации

### Общий интерфейс

Программы с использованием каждой технологии имеют одинаковый интерфейс.

Программа считывает из потока ввода параметры:
* M — размер сетки по оси _x_, целое число, M > 2;
* P — размер сетки по оси _t_, целое число, P > 1;
* h — шаг сетки по оси _x_, вещественное число, h > 0;
* tau — шаг сетки по оси _t_, вещественное число, tau > 0;
* a — величина обратная фазовой скорости, вещественное число, 
|tau*a/h| < 1 для сходимости схемы.
* phi1 — начальное значение координаты, массив вещественных чисел 
1..M;
* phi2 — значение производной координаты, массив вещественных чисел 
1..M;
* psi1 — значение левой границы, массив вещественных чисел 1..P;
* psi1 — значение правой границы, массив вещественных чисел 1..P.

После вычисления программа выводит в поток вывода решение — массив чисел P x M 
в формате P строк, каждая строка состоит из M вещественных чисел через пробел.

Каждую программу можно собрать (в случае реализации на Python — запустить) с 
опцией измерения скорости. В этом случае программа не будет вводить из потока 
ввода краевые условия задачи и не будет выводить результат в поток вывода, 
краевые условия будут сгенерированы внутри программы. 

Интерфейс программ на С реализован в файлах [common/io.h](/solution/common/io.h) и 
[common/io.c](/solution/common/io.c).

### Проверка корректности работы

Принцип тестирования описан в разделе [Тестирование](#тестирование). 

Исходные данные для тестирования хранятся в файл 
[common/test.inputs.txt](/solution/common/test.inputs.txt). Они подаются на вход
программе, после чего ее вывод передается программе [тестеру](/test/test_results.py),
которая сравнивает вывод с эталоном. Эта программа выводит среднюю и максимальную
разницу между выходами, тест считается пройденным, если максимальная разница не 
превышает _5e-3_.

### Проведение экспериментов

Запуск программы с различными параметрами и измерение времени исполнения 
было реализовано скриптом [common/runner.py](/solution/common/runner.py).

Программа принимает на вход параметры эксперимента в следующем формате:
* строка 1: одно целое число — количество повторений каждого измерения;
* строка 2: последовательность целых чисел — параметры _M_;
* строка 3: последовательность целых чисел — параметры _P_;
* строка 4: одно вещественное число — параметр tau;
* строка 5: одно вещественное число — параметр h;
* строка 6: одно вещественное число — параметр a;
* строка 7: последовательность целых чисел — параметры _N_.

Числа в последовательности разделены пробелом или отступом.

Скрипт измеряет время выполнения с помощью встроенной утилиты `time`.
Программа выводит в поток вывода таблицу данных в формате `csv` со следующими 
столбцами:
* `id` — номер запуска, целое число;
* `M` — параметр _M_, целое число;
* `P` — параметр _P_, целое число;
* `NP` — количество потоков/процессов, целое число;
* `user` — вывод "user time" утилиты `time`, вещественное число, время в секундах;
* `system` — вывод "system time" утилиты `time`, вещественное число, время в 
секундах;
* `elapsed` — вывод "real time" утилиты `time`, вещественное число, время в 
секундах.

Настройки для экспериментов с 1, 2 и 5 узлами (см. 
[исследование скорости работы](#исследование-скорости-работы)) находятся в 
каталоге [common/experiment](/solution/common/experiment/).

### Интерфейс сборки и запуска

Каждая программа имеет одинаковый интерфейс сборки и запуска в 
различных режимах, реализованный с помощью технологии Makefile.

Каждая программа имеет следующие цели:
* rebuild — пересобрать все объектные файлы;
* run — запустить программу в интерактивном режиме, аргументом `NP` 
можно указать количество потоков/процессов, по умолчанию — 4.
* test — проверить корректность работы программы.
* speedtest — замер скорости работы на одном узле, вывод результатов 
в поток вывода.

Программы с технологией MPI имеют следующие цели:
* speedtest2 — замер скорости работы на двух узлах; 
* speedtest5  — замер скорости работы на пяти узлах.

Эти цели не выделяют узлы, а только проводят эксперимент с соответствующими 
параметрами. Выделить узлы нужно извне.

### Технология POSIX Threads

Для этой технологии был реализован 
[алгоритм с общей памятью](###-Параллельный-алгоритм-с-общей-памятью).

Каждый поток обрабатывал отдельные элементы вектора, основываясь на его номере 
_r_. Так в задаче с размером вектора _M_ и количеством потоков _N_ потоков с 
номером _r_ обрабатывал элементы _r_, _r + N_, _r + 2N_ и т.д.

В [алгоритме](#последовательный-алгоритм-расчета) потоки должны 
синхронизироваться после шага 3 и на каждой итерации шага 4. Это может быть 
достигнуто различными способами: с помощью ожидания завершения потоков и с 
помощью барьера.

Оба варианта были реализованы: решение с ожиданием завершения работы потоков
в файле [solution_join.c](/solution/pthreads/solution_join.c), решение с 
барьерами в файле [solution_barrier.c](/solution/pthreads/solution_barrier.c), 
костяк программы в файле [solution_phreads.c](/solution/pthreads/solution_pthreads.c).

Количество потоков передается первым аргументом для запуске, если этот аргумент
отсутствует или не является целым числом > 0, то используется значение 4.

Вычисления значений по формулам (2) и (3) были вынесены в отдельный файл 
[common/iteration.c](/solution/common/iteration.c).

Как и ожидалось, версия с ожиданием завершения работы потоков работает ощутимо 
медленнее из-за необходимости пересоздавать потоки большое количество раз, 
поэтому по умолчанию при сборке используется версия с барьерами, однако с помощью 
[Makefile](/solution/pthreads/Makefile) можно собрать и запустить версию с 
ожиданием завершения.

### Технология OpenMP

Для этой технологии был реализован 
[алгоритм с общей памятью](###-Параллельный-алгоритм-с-общей-памятью).

Каждый поток обрабатывал отдельные элементы вектора с помощью директивы 
`parallel for`. Таким образом были распараллелены вычисления на шагах 1-3 и 
на каждой итерации шага 4.

Программа реализована в файле [solution_omp.c](/solution/openmp/solution_omp.c),
она, как и другие программы использует функции из 
[common/iteration.c](/solution/common/iteration.c).

Количество процессов передается первым аргументом для запуске, если этот аргумент
отсутствует или не является целым числом > 0, то используется значение 8. 
Это количество явным образом указывается в каждой директиве `parallel for`.

### Реализация MPI 

Программы с использованием MPI работали следующим образом.
1. Главный процесс считывает параметры задачи и начальные и краевые условия из 
потока ввода.
2. Главный процесс широковещательно рассылает параметры задачи.
3. Начальные условия распределяются по алгоритму распределения, описанному в этом 
[разделе](#параллельный-алгоритм-с-распределенной-памятью). Каждый процесс 
получает последовательную часть данных по оси _x_. 
4. Каждый процесс вычисляет каждый следующий ряд, получив от соседей краевые 
значения предыдущего ряда. Обмен краевыми условиями происходит при помощи
операции "Send-receive".
5. Все результаты отправляются обратно главному потоку, который распечатывает 
итоговый массив в поток вывода.

Распределение начальных условий и сбор итоговых данных проходил по алгоритму,
описанному в [разделе](#параллельный-алгоритм-с-распределенной-памятью). 
Единственным отклонением является то, что устанавливается ограничение на 
количество потоков: размерность обрабатываемых данных без границ (то есть 
_M - 2_) должно быть хотя бы в два раза больше, чем количество потоков для 
корректной работы алгоритма обмена краевыми условиями. Лишние потоки в этом 
случае просто отключаются.

Реализация на C выполнена в файле 
[cmpi/solution_mpi.c](/solution/cmpi/solution_mpi.c), реализация на Python 
выполнена в файле [pympi/solution_mpi.py](/solution/pympi/solution_mpi.py).

### Обработка данных

Для обработки данных использовался язык R, с помощью него были посчитаны средние
значения для всех однотипных измерений, после чего были построены графики.

Программа обработки данных сохранена в файле [analysis.R](/analysis/analysis.R).

## Результаты работы программы

На рисунках ниже представлены результаты измерений для различных технологий на 
одном узле.

![Результаты измерения POSIX threads](/report/images/pthreads1.png)
![Результаты измерения OpenMP](/report/images/openmp1.png)
![Результаты измерения C MPI](/report/images/cmpi1.png)
![Результаты измерения Python MPI](/report/images/pympi1.png)

По результатам измерения реализации POSIX threads и OpenMP видно, что максимум 
скорости достигается при использовании многопоточности, но при использовании 
количества потоков больше 28, то есть больше количества ядер на процессоре,
время заметно ухудшается. Для OpenMP оптимальное количество потоков равно 14 
для задач низкой размерности и 28 для задачи самой большой размерности. Для 
технологии POSIX threads заметно, что оптимальное значение количества потоков
равно 4 для самых маленьких задач и равно 14 для задач большей размерности. 
Связать это можно с тем, что процесс создание потоков занимает некоторое время,
которое имеет меньший вклад в общее время решения задачи для большой размерности.

По результатам замеров для MPI видно, что задачи решаются ощутимо дольше, чем 
при других технологиях. Это связано с тем, что объем пересылаемых данных по 
итогу практически совпадает с объемом данных, обрабатываемых довольно простой 
операцией. Таким образом обмен данными получается невыгодным с точки зрения 
производительности. Также на каждой итерации процессам нужно обменяться с 
соседями граничными условиями, что требует синхронизации всех процессов. Это 
может повлечь уменьшение производительности при увеличении числа процессов. 

По результатам для технологии C MPI видно, что увеличение количества процессов 
имеет преимущества только при большой размерности задачи. Для задач меньшей 
размерности увеличение количества процессов дает совсем небольшое улучшение.

Реализация на Python еще хуже справляется с пересылкой данных, что видно из 
того, что во всех случаях случай с один процессом, при котором не происходит 
пересылки данных работает быстрее, чем с несколькими процессами. Это, вероятно, 
происходит из-за того, что в Python происходит избыточное копирование данных
при получении, поскольку в Python сложнее работать с памятью напрямую.

По итогам измерения на одном узле можно сказать, что быстрее всего работает 
реализация с технологией OpenMP.

Ниже представлены сравнения запуска MPI на C и Python на 2 и 5 узлах.

![Результаты работы MPI на 2 узлах](/report/images/mpi2.png)
![Результаты работы MPI на 5 узлах](/report/images/mpi5.png)

В эксперименте с двумя узлами можно наблюдать результаты, схожие с результатами
запуска на одном узле, и для Pyhton, и для C видны те же тенденции. Можно 
только отметить, что реализация на C работает ощутимо быстрее реализации на 
Pyhton.

В эксперименте с пятью узлами заметно видно тенденцию увеличения времени работы 
при увеличении числа процессов, которая начинается на прошлых графиках с 28 
процессов. Видимо, это происходит из-за блокирующей передачи при обмене краевыми 
условиями на каждой итерации.

## Заключение

В рамках работы был разработано 2 параллельных алгоритма решения одномерного 
волнового уравнения: с общей памятью и с распределенной памятью. Эти алгоритмы
были реализованы с помощью технологий POSIX Threads, OpenMP, MPI на С и MPI на 
Python. Программы были запущены с различными конфигурациями на суперкомпьютере 
"Политехнический", было измерено время их выполнения. Измерения были проведены 
на 1, 2 и 5 узлах.

Для одного узла самой быстрой оказалась реализация OpenMP. Реализации MPI 
работали медленнее решений с общей памятью. Для экспериментов с несколькими 
узлами реализация с MPI на С работала быстрее, чем реализация на Python. К 
сожалению, при увеличении количества потоков, реализация работала хуже. 
Возможной причиной является блокирующие операции пересылки на каждой итерации.