Этот чат:

Создаёт комнату для пользователей
Регистрацию пользователей
Передаёт сообщения всем пользователям комнаты
Запоминает юзернейм пользователя
Отдаёт историю сообщений late-join пользователям (можно крутить ручку по глубине очереди)


Этот чат не:
Имеет своих эксепшенов
Длительного запоминания пользователей


REGISTER username password\n
LOGIN username password\n
MSG text...\n
Flow:

Клиент подключился

Должен отправить REGISTER или LOGIN

Если успех → AUTH_OK

Если нет → AUTH_FAIL

После AUTH_OK можно отправлять MSG


Переход стейтов
read line

if state == WaitingAuth:
    parse REGISTER / LOGIN
    if success:
        state = Authenticated
        room.join(...)
    else:
        send AUTH_FAIL

else if state == Authenticated:
    broadcast message


