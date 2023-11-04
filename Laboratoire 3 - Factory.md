
Auteurs: Kevin Auberson, Patrick Maillard
## Description des fonctionnalités du logiciel
Le logiciel de simulation de marché a pour but de modéliser les interactions entre trois types d'entités : les extracteurs de ressources, les usines de production et les grossistes. Chaque entité joue un rôle spécifique dans la chaîne de production de robots, depuis l'extraction des matières premières jusqu'à la vente des produits finis. Les fonctionnalités clés du logiciel comprennent :
 
### Extracteurs de Ressources  
Les extracteurs simulent l'extraction de ressources telles que le cuivre, le sable et le pétrole. Ils emploient des mineurs pour effectuer l'extraction. Les fonctionnalités associées comprennent l'embauche d'employés, l'extraction de ressources, et la vente de ces ressources aux grossistes.

### Usines
Les usines sont responsables de la production d'objets tels que les puces électroniques, le plastique et les robots. Elles utilisent des matières premières pour produire ces objets et emploient des travailleurs. Les usines peuvent acheter les matières premières nécessaires aux grossistes. Les fonctionnalités comprennent la production d'objets, l'achat de matières premières et la gestion des employés.

### Grossistes 
Les grossistes jouent un rôle intermédiaire en achetant des matières premières aux extracteurs et des produits aux usines. Ils revendent ensuite ces ressources et produits à un prix plus élevé. Les grossistes gèrent leurs propres stocks et fonds. Les fonctionnalités incluent l'achat de matières premières, la vente de produits et la gestion des finances.
## Choix d'implémentation 
La gestion de la concurrence a été le problème principale dans le projet, car plusieurs threads doivent interagir simultanément. Nous avons utilisé des mutex  pour verrouiller l'accès aux stocks et aux fonds, garantissant que les opérations se déroulent de manière cohérente.
### Extracteurs de Ressources
Chaque extracteur exécute une routine `run` qui simule le processus d'extraction. La gestion de la concurrence a été mise en place dans la fonction `run`à l'aide de mutex pour garantir des opérations sûres sur les stocks et les fonds.
### Usines
Les usines vérifient si elles ont suffisamment de ressources pour produire des objets. Si ce n'est pas le cas, elles achètent les ressources nécessaires aux grossistes en utilisant la méthode `orderResources`. Les usines peuvent également produire des objets en payant leurs employés et en utilisant la méthode `buildItem`.  La gestion de la concurrence est assurée à l'aide de mutex dans ces deux fonctions.
### Grossistes
Les grossistes peuvent acheter des ressources auprès des extracteurs et des usines en utilisant la méthode `buyResources`. La gestion de la concurrence est assurée à l'aide de mutex.


## Tests effectués


Description de chaque test, et information sur le fait qu'il ait passé ou non