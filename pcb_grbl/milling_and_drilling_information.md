# Usinage PCB ampli milliohmmetre
Dimensions PCB : 61.6 x 48.9 mm
Entraxe trous de fixation : 54.0 x 41.3 mm
Routage PCB finalisé avec Kicad le 09/04/25
Prise en compte amélioration pads pour fil plus large, réutilisation maquette PCB INA199
Largeur piste 0.6 sauf au voisinage du INA199 : 0.4 mm
Isolation min : 0.4 mm
Utilisation plans de masse et Vcc avec isolation 0.4 mm.
Fichiers cnc générés avec Flatcam le 09/04/25, repris le 26/04/25 suite à 3 pistes oubliées dans Kicad
Fichiers gcode contrôlés avec utilitaire python le 09/04/25, recontrôlés le 26/04/25
Fichiers d'entrée Flatcam tournés de 90° pour que l'axe Y soit selon la plus grande longueur.
Origine usinage : en bas à gauche du PCB


# A faire dans cet ordre :
- fixer le PCB
- passer en mode avion
- charger PCB_milli-Edge_Cuts.gbr_cutout.nc pour avoir les contours du circuit
- faire le zero X et Y et noter le 0 machine
- monter fraise javelot et la sonde Z, faire le 0Z
- faire le height map après avoir validé auto pour reconnaitre les contours
- enlever la sonde Z
- appliquer couche d'huile
- lancer PCB_milli-B_Cu.gbr_iso.nc avec fraise javelot => isolation des pistes en 4 passes
- lancer PCB_milli-B_Cu.gbr_ncc.nc avec fraise javelot => nettoyage petits ilots de cuivre
- monter foret 0.8 mm et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PCB_milli-PTH.drl_cnc_0.8.nc => perçage trous 0.8 mm
- monter foret 1 mm et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PPCB_milli-PTH.drl_cnc_1.0.nc => perçage trous 1.0 mm
- monter foret 1.25 mm et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PCB_milli-PTH.drl_cnc_1.25.nc => perçage trous 1.25 mm
- monter foret 2.5 mm et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PCB_milli-PTH.drl_cnc_2.5.nc => perçage trous 2.5 mm
- monter foret 3 mm et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PCB_milli-PTH.drl_cnc_3.0.nc => perçage trous 3.0 mm
- monter fraise corm shaped 0.95 et la sonde Z et faire le 0Z
- enlever la sonde Z
- lancer PCB_milli-Edge_Cuts.gbr_cutout.nc : découpe du PCB

