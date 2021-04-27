# api_2019

Final project of the course "Algoritmi e Principi dell'Informatica", Politecnico di Milano, AY 2018-2019.

## Overview
The task is to construct a single file C program for monitoring entities and relationships between them. Entities have a name and relationships have a type, an origin and a destinations (they are not symmetrical). An new entity or relationship can be added to the system and existing ones can be removed. The system should also implement a command 'report' which returns, for each type of monitored relationship, the entity which receives most relationships of that type. These are the commands the system should implement:

- addent "name" : start monitoring entity "name", if "name" is already being monitored, do nothing
- addrel "origin" "destination" "type" : add relationship "type" between "origin" and "destination", if already existing or if one of the two entities is not being monitored by the system, do nothing
- delent "name" : stop monitoring entity "name", also remove all relationships that used to connect to "name"
- delrel "origin" "destination" "type" : remove relationship "type" between "origin" and "destination"
- report : for each type of relationship monitored by the system, print the name of the entity receiving most instances of that relationship type and the number of instances, in case of ties, print all names in alphabetical order
