.386 ;directive qui indique pour quelle processeur il faut assembler le code
.model flat, stdcall
option casemap : none

include \masm32\include\masm32rt.inc


.code


salut :

mov eax, 16b
print eax
inkey "Appuyer sur une touche pour continuer"
exit  
 
end salut
