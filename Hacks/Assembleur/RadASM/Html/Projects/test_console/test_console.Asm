.386 ;directive qui indique pour quelle processeur il faut assembler le code
.model flat, stdcall
option casemap : none

include \masm32\include\masm32rt.inc


.code


salut :

print "Salut les zeros!", 13, 10
print "Contenu de eax : " 
mov eax, 16d

print str$ (eax), 13, 10
inkey "Appuyer sur une touche pour continuer"
exit  
 
end salut
