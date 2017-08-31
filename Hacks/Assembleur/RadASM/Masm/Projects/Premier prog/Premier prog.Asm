.386                       ;directive  indiquant qu'il s'agit du prossesseur 80386
.model flat, stdcall       ;directive inconu
option casemap : none      ;directive indiquant qu'il faut faire la différence entre les majuscules et les minuscules

include \masm32\include\masm32rt.inc     ;encore une directive inconnu


.code          ;début du code


salut :             ;label de début du code

print "contenu de ch : "       ;affiche "contenu de ch"

mov ch, 40                     ;met 40 dans ch



   movzx eax, ch                 ;extend la variable de registre ch pour la mettre dans eax
   print str$ (eax)              ;affiche la variable de registre eax qui contient ch
    
print " ", 13, 10                ;effectue un retour à la ligne
print "contenu de cl : "
mov cl, 35                      ;met 35 dans cl 



   movzx eax, cl                ;extend la variable de registre cl pour la mettre dans eax
   print str$ (eax)             ;affiche la variable de registre eax qui contient cl

print " ", 13, 10               ;effectue un retour à la ligne

 inkey "Appuyez sur une touche pour continuer..."   ;met le programme en pause avec le message "Appuyez sur une touche pour continuer..."
 exit                                               ;quitte le programme
 
end salut                                           ;ferme le label
