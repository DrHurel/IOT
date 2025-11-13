Faire un system d'automatisation d'arosage de plante, pour une plante donnée en fonction de ses besoins on va venir observer grace à un controller des variables d'environement pour venir déclancher l'arosage en fonction de seuil.

On veut pourvoir suivre les constante de la plante à distance grave à un application mobile et etre en mesure de déclanger l'arosage de manière manuel depuis cette meme application

Sur le controller on aura aussi une indicatio visuel des valeur des variables.

les variables collectable sont :
- température
- humidité
- luminausité

les seuils définisable pour une plante sont
- température optimal
- humidité optimal de la pièce
- cycle entre 2 arosage
- quantité d'eau pour cycle d'arosage

Le serveur va venir permettre de stocker l'historique des mesures
ainsi qu'un systeme d'authentification
On stockera aussi les configuration propre à chaque plantes
le serveur pourra recevoir des updates du microcontroler
le server pourra déclancher l'arosage automatiquement
pour un utilisateur on pourra récuper la liste de ses plantes

le controller aura pour unique fonctionnalité de mettre à disposition
- la liste de ses captueur
- envoyer ses données sur demande si besoin
- déclancher l'arosage
- s'appareillé avec un téléphone en bluetooth pour la 1ere configuration (wifi)
- se connecter à un réseau wifi

POur l'application mobile
on a un menu de connection
    création nouveau compte
    se connection
    déconnection

la liste de config de plante 
la liste des plante configuré 
ajouter une nouvelle plante
localisation gps
pour une plante donnée on peut démarrer l'arosage

