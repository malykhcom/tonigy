/* */

tmp='tmp.bod'
sender='e:\soft\pmmail\pmmsend.exe'
parse arg a
parse var a email name
say 'Генерируем письмо для "'name'" ('email')'
'@keygen "'name'" | rxqueue'
key=''
do queued()
    parse pull a
    key=key||a||'0d0a'x
end

'@del 'tmp
call lineout tmp, 'Уважаемый(ая) 'name'!'
call lineout tmp, ''
call lineout tmp, 'Благодарим Вас за регистрацию Tonigy. Для того, чтобы снять ограничения'
call lineout tmp, 'незарегистрированной версии, пожалуйста, воспользуйтесь ключом:'
call lineout tmp, ''
call lineout tmp, key
call lineout tmp, 'Скопируйте эти две текстовые строки в clipboard (промежуточный буфер).'
call lineout tmp, 'Затем запустите программный объект "Register Tonigy" (или выполняемый'
call lineout tmp, 'файл register.exe из директории, куда был установлен Tonigy) и вставьте'
call lineout tmp, 'содержимое clipboard при помощи комбинации клавиш Shift+Ins. После'
call lineout tmp, 'этого нажмите кнопку "Ok" и перезапустите Tonigy, если он бы запущен.'
call lineout tmp, ''
call lineout tmp, 'Если у Вас возникли какие-нибудь предложения, замечания или вопросы, '
call lineout tmp, 'пожалуйста, обращайтесь в службу технической поддержки '
call lineout tmp, '(tonigy@defgroup.com). Обратите внимание, что Вы можете использовать'
call lineout tmp, 'русский язык для общения со службой технической поддержки.'
call lineout tmp, ''
call lineout tmp, 'DEF Group.'
call lineout tmp, 'Tonigy home page: http://www.tonigy.com'
call stream tmp, 'c', 'close'

'@'sender' /m 'directory()'\'tmp' "'name' <'email'>" "Tonigy Registration" TONIGY0.ACT'