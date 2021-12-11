# TP TCP-CHAT
- Tchat Client-Serveur en C 

## Compilation et Exécution
- Pour compiler: make
- Pour exécuter le serveur: ./serverThread [n°port]
- Pour exécuter le serveur: ./serverThread [@IP] [n°port] [pseudo](on prendra comme @IP: 127.0.0.1 si on souhaite tester le serveur en local)
- Pour afficher la liste des utilisateurs connectés sur le serveur: "Liste"
- Pour se connecter à un utilisateur et lui envoyer des messages: "Envoi [pseudo]"
- Pour sortir de la discussion en cours: "Quit"
- Pour quitter le serveur: "Exit"

## Ce que nous n'avons pas fait:
- Résolution du nom de serveur (exemple: mandelbrot) en adresse IP
- Nous n'avons pas réussi à mettre en place la commande "Envoi [pseudo] [message]" (le message s'affiche seulement quand le client récepteur  