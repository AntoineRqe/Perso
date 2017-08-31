.386                       ;directive  indiquant qu'il s'agit du prossesseur 80386
.model flat, stdcall       ;directive inconu
option casemap : none      ;directive indiquant qu'il faut faire la différence entre les majuscules et les minuscules; ceci est un programme affichant
; simplement "Hello World"
include \masm32\include\masm32rt.inc     ;encore une directive inconnu
mov  dx, texte
mov  ah, 21
int 21      ; afficher le hello world
exit         ; fin du programme

texte: db 'Hello, World !!', 10, 13, '$'
