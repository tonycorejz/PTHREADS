# PTHREADS
Существует 5 магазинов, 1 погрузчик и 3 покупателя. В каждом магазине изначально 1000 товаров. Погрузчик грузит поочередно
500 товаров в каждый магазин если он не занят покупателем, а в противном случае идет в следующий. Если все магазины заняты,
погрузчик едет на склад, а затем возвращается. Покупатели заходят в магазины одновременно, но в одном магазине должен быть только
один покупатель. Если магазин занят другим покупателем или погрузчиком, покупатель идет в следующий, в случае, когда
все магазины заняты, покупатель идет домой, и возвращается обратно.Если покупатель зашел в магазин, он выкупает весь товар.
Все эти действия происходят до того момента, пока все покупатели не выкупят 10000 товаров каждый.