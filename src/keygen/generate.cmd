/* */

tmp='tmp.bod'
sender='e:\soft\pmmail\pmmsend.exe'
parse arg a
parse var a email name
say '������㥬 ���쬮 ��� "'name'" ('email')'
'@keygen "'name'" | rxqueue'
key=''
do queued()
    parse pull a
    key=key||a||'0d0a'x
end

'@del 'tmp
call lineout tmp, '��������(��) 'name'!'
call lineout tmp, ''
call lineout tmp, '�������ਬ ��� �� ॣ������ Tonigy. ��� ⮣�, �⮡� ���� ��࠭�祭��'
call lineout tmp, '����ॣ����஢����� ���ᨨ, ��������, ��ᯮ������ ���箬:'
call lineout tmp, ''
call lineout tmp, key
call lineout tmp, '�������� �� ��� ⥪�⮢� ��ப� � clipboard (�஬������ ����).'
call lineout tmp, '��⥬ ������� �ணࠬ��� ��ꥪ� "Register Tonigy" (��� �믮��塞�'
call lineout tmp, '䠩� register.exe �� ��४�ਨ, �㤠 �� ��⠭����� Tonigy) � ��⠢��'
call lineout tmp, 'ᮤ�ন��� clipboard �� ����� �������樨 ������ Shift+Ins. ��᫥'
call lineout tmp, '�⮣� ������ ������ "Ok" � ��१������ Tonigy, �᫨ �� �� ����饭.'
call lineout tmp, ''
call lineout tmp, '�᫨ � ��� �������� �����-����� �।�������, ����砭�� ��� ������, '
call lineout tmp, '��������, ���頩��� � �㦡� �孨�᪮� �����প� '
call lineout tmp, '(tonigy@defgroup.com). ����� ��������, �� �� ����� �ᯮ�짮����'
call lineout tmp, '���᪨� �� ��� ��饭�� � �㦡�� �孨�᪮� �����প�.'
call lineout tmp, ''
call lineout tmp, 'DEF Group.'
call lineout tmp, 'Tonigy home page: http://www.tonigy.com'
call stream tmp, 'c', 'close'

'@'sender' /m 'directory()'\'tmp' "'name' <'email'>" "Tonigy Registration" TONIGY0.ACT'