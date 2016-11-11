# 12.11.2016

`bitstream.h` - структура для побитового чтения из строки.

`BitStream_ReadInt(BitStream* bitStream, size_t bitCount)` - единственная функция для чтения данных.
возвращает long long int. `bitCount` не может быть больше 64.

Все остальное это дебаг и логирование.

# Результат

Сама по себе *упаковка* данных не помогла решить проблему генерации валидных данных. Поток данных генерируемый afl слишком мал в результате постоянно получаем ошибку "not enough bits". Единственный способ сообщить afl что нам нужен *input* побольше это запихать в initial tests огромный уже *валидный* тест или догенерировать недостающую часть руками в afl_postprocess: 
[talk 1](https://groups.google.com/forum/#!topic/afl-users/2AByjTg2gaM), [talk 2](https://groups.google.com/forum/#!msg/afl-users/mZERmpBc2to/Xu8QTcZv-98J;context-place=forum/afl-users)

Можно еще попробовать зациклить поток данных.
