/* */

tmp='tmp.bod'
sender='e:\soft\pmmail\pmmsend.exe'
parse arg a
parse var a number name
say '������㥬 ���쬮 ��� "'name'" (��� 'number')'
'@keygen "'name'" | rxqueue'
key=''
do queued()
    parse pull a
    key=key||a||'0d0a'x
end

'@del 'tmp
call lineout tmp, 'Hello.'
call lineout tmp, ''
call lineout tmp, '�������樮��� ��� (2 ⥪�⮢�� ��ப� �� 44 ᨬ���� � ������):'
call lineout tmp, ''
call lineout tmp, key
call lineout tmp, 'Anton Malykh.'
call lineout tmp, 'DEF Group.'
call stream tmp, 'c', 'close'

'@'sender' /m 'directory()'\'tmp' nalp@swrub.com "'number'" SUPPOR0.ACT'
