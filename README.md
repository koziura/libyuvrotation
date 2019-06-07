# Image-Rotator YUV 4:2:0

### Состав проекта:
a) libyuvrotation – динамическая библиотека поворота изображений;  
b) testlibyuvrotate – консольное приложение, работающее с библиотекой libyuvrotation.

### Подготовка исходных текстов проекта:  
Необходимый минимум: `Cmake2.8, C++11, boost-1.55, jpeg, uuid, pthread`;  
В корневом каталоге исходных текстов (`%your_some_path%/libyuvrotation`) создать директорию `build`.  
В терминале задать путь к проекту вида `%your_some_path%/libyuvrotation/build`  
  
### Сборка исходных текстов проекта:  
* **Release**: `cmake .. && make`  
* **Debug**: `cmake –DCMAKE_BUILD_TYPE=Debug .. && make`  
  
### Запуск проекта:  
* В каталоге проекта `%your_some_path%/libyuvrotation/bin`, после успешной компиляции будут подготовлены исполняемые файлы `testlibyuvrotate, libyuvrotation`  
* Запуск производится путем ввода команды в терминале `./testlibyuvrotate`  
* Программа работает с командными аргументами:  
`* help {begin} *`  
`* Welcome Image-Rotator YUV 4:2:0 *`  
`[arguments]`  
`--i [input path] path for scan source yuv images.`  
`--o [output path] path for write rotated yuv images.`  
`--w [width] set width value source yuv image.`  
`--h [height] set height value source yuv image.`  
`--r [rotate] set rotate mode (90/180/270).`  
`--t [threads] set thread count for rotation performance.`  
`(default: all available CPU-threads)`  
`[results]`  
`the output of results will be written thread address with`  
`performance time for once image.`  
`for stop process press Ctrl+C.`  
`[tests]`  
`--b [unit-test] set benchmark iterations.`  
`-h [gtest] output googletest helper (set with '--b' arg).`  
`* help {end} *`  
  
Для запуска программы в ОБЫЧНОМ РЕЖИМЕ, необходимо выполнить команду:  
`./testlibyuvrotate --i ./ --o ./out --w 1400 --h 1416 --r 90`  
  
### Общий принцип работы
  Выполнение команды для обычного режима будет означать, что в каталоге запуска программы должны находиться исходные изображения для дальнейшего преобразования.  
  
  Исходная директория должна существовать.  
  Выходная директория будет расположена в директории запуска с подкаталогом out.  
  Разрешение исходного изображения будет задано: `1416х1400` пикселей, с углом поворота изображений на `90 ̊ `градусов.  
  Для примера, директория запуска содержит изображение `mark.yuv` для демонстрации работы программы.  
  После выполнения преобразования, в выходной директории должен получиться файл с добавленным префиксом rotate90_ к оригинальному имени исходного изображения.  
  Во время выполнения программы, производится детектирование изменений в исходном каталоге, таким образом, что если в данном каталоге будут записываться файлы, то они будут сразу отправлены на обработку поворота.  
  Исходный файл должен иметь расширение `“yuv”`, задаваемое разрешение исходногоизображения должно соответствовать разрешению исходного файла, иначе такое изображение не будет обработано, а на экран будет выведено соответствующая информация об ошибке.  
  По умолчанию, программа использует аппаратные возможности `CPU` для одновременного запуска возможных параллельных потоков обработки изображений. Этот параметр может быть уменьшен через аргумент запуска `--t` с необходимым зачтением.  
  Для запуска программы в `РЕЖИМЕ ТЕСТ`, необходимо добавить к стандартному набору аргументов параметр `--b`. В данном режиме будет запущен симулятор исходных изображений, который будет создавать файлы типа `24aebec0-150d-4ae8-baad-319a1ee9e31c.yuv`.  
  На основе уникальных значений (`uuid`). Исходная директория может быть создана автоматически. В выходной директории будут обработанные изображения типа: `rotate90_24aebec0-150d-4ae8-baad-319a1ee9e31c.yuv`.  
  Режим симуляции использует технологию `uinit-test` от `google-test framework`, для вызова справочной информации добавьте аргумент `-h`.
  Чтобы использовать режим симуляции в многократном использовании, необходимо добавить аргумент `--gtest_repeat=50`. Это будет означать, что команда симуляции будет выполнена `50` раз. Также, в данном режиме, программа проверяет критические моменты на предмет записи выделения и освобождения памяти.  
  Остановка процесса выполнения программы осуществляется путем нажатия клавиш клавиатуры **Ctrl+C**.  
