# Analyse statistique d'une liste de mots de passe

```shell
git clone https://git.unicaen.fr/passwords/cpack
```

## Requiert

Une liste de mots de passe sous forme d'un fichier .txt, et au format UTF-8.

Pour convertir un fichier .txt au format UTF-8, utilisez la commande suivante :

```shell
iconv -f ISO-8859-1 -t UTF-8 input.txt -o output.txt
```


## Programme version console

Pour analyser une liste de mot de passe, effectuez les commandes suivantes :

```shell
cd Statsgen
make cpack
./cpack liste.txt
```

Plusieurs options sont disponibles :
 * --help, -h
 * --withcount, -w     : préciser que la liste est au format "withcount" (i.e. [nombre d'occurences] [mot de passe])
 * --hiderare, -hr     : cacher les statistiques inférieures à 1%
 * --top, -t [value]   : montrer que les [value] premiers résultats
 * --regex, -r [value] : montrer les résultats des mots de passe respectant l'expression régulière [value]

Options d'optimisation :
 * --limitadvancedmasks, -lam [value] : limiter la taille des masques avancées à [value], si taille>[value]: othermasks
 * --limitsimplemasks, -lsm [value]   : limiter la taille des masques simples à [value], si taille>[value]: othermasks
 * --parallel, -p [value] 	      : nombre de thread utilisables (1 par défaut)
    
Règles de sécurité :
 * --security, -s : définir les règles de sécurités

Par défaut, les règles de sécurité sont les suivantes :
 * Taille minimale des mots de passe : 8
 * Minimum 1 minuscule, 1 majuscule, 1 chiffre


## Interface graphique

Une interface en QT est disponible pour une version simplifiée et visuelle des statistiques.

	--> Requiert la version de QT comprenant QtCharts (version 5 minimum)
	--> Programme développé en version 5.9.4

```shell
cd Statsgen
make cpack-gui
./cpack-gui
```


## Fonctions utilitaires

Dans le dossier Utils, trois programmes ont été créées
 * convert.cpp : permet de créer une liste de mots de passe à partir d'une liste au format withcount
 * createNewDB.cpp : permet de créer des listes de mots de passe plus ou moins grandes, utile pour nos tests
 * memory.sh : permet de connaître la mémoire vive utilisée pour un certain processus

## License

Voir [LICENSE](LICENSE) pour plus de détails.
