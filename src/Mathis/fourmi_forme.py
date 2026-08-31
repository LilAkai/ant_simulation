import math
import sys as _sys
 
if "PySide6.QtGui" in _sys.modules:
    from PySide6.QtCore import Qt
    from PySide6.QtGui import QColor, QPainterPath
elif "PyQt6.QtGui" in _sys.modules:
    from PyQt6.QtCore import Qt
    from PyQt6.QtGui import QColor, QPainterPath
elif "PyQt5.QtGui" in _sys.modules:
    from PyQt5.QtCore import Qt
    from PyQt5.QtGui import QColor, QPainterPath
else:
    try:
        from PySide6.QtCore import Qt
        from PySide6.QtGui import QColor, QPainterPath
    except ModuleNotFoundError:
        try:
            from PyQt6.QtCore import Qt
            from PyQt6.QtGui import QColor, QPainterPath
        except ModuleNotFoundError:
            from PyQt5.QtCore import Qt
            from PyQt5.QtGui import QColor, QPainterPath
 
# ---- réglages globaux ----------------------------------------------------
COULEUR = QColor("#33291f") # couleur du corps
FOND = QColor("#e8e2d5")         # couleur du fond de l'atelier
EPAISSEUR = 0.0                  # 0 = aucun contour, tout est rempli
COULEUR_YEUX = QColor("#000000") # couleur des yeux
 
 
# ==========================================================================
#  LES OUTILS  (pas besoin d'y toucher)
# ==========================================================================
 
def symetrie(sommets):
    """La même liste de points, renvoyée de l'autre côté du corps."""
    return [(x, -y) for x, y in sommets]
 
 
def miroir(demi):
    """Ferme une demi-forme en lui recollant son reflet.
 
    On donne le contour du HAUT seulement ; la fonction rajoute le bas,
    parcouru en sens inverse pour que le polygone se referme proprement.
    Les points posés sur l'axe (y = 0) ne sont pas dupliqués : ce sont les
    pointes, elles appartiennent aux deux moitiés à la fois.
 
    C'est CE mécanisme qui rend la symétrie impossible à casser.
    """
    reflet = [(x, y) for x, y in symetrie(demi)[::-1] if y != 0]
    return demi + reflet
 
 
def aire_signee(sommets):
    """Aire du polygone, AVEC un signe : positif s'il est parcouru dans un
    sens, négatif dans l'autre. C'est la façon de savoir dans quel sens
    tourne un contour."""
    total = 0.0
    for i, (x1, y1) in enumerate(sommets):
        x2, y2 = sommets[(i + 1) % len(sommets)]
        total += x1 * y2 - x2 * y1
    return total / 2
 
 
def polygone(chemin, sommets):
    """Ajoute un polygone fermé, TOUJOURS parcouru dans le même sens.
 
    Pourquoi ce detour : la regle de remplissage compte les tours. Deux
    contours qui tournent dans le MEME sens s'additionnent (l'union est
    pleine) ; deux contours qui tournent en sens CONTRAIRE s'annulent la
    ou ils se chevauchent, et on voit apparaitre des encoches de fond a
    l'attache des pattes ou sous les mandibules. Comme une patte du bas
    est le reflet d'une patte du haut, elle tourne forcement a l'envers :
    sans cette remise a l'endroit, la moitie des chevauchements se troue.
    """
    if aire_signee(sommets) < 0:
        sommets = sommets[::-1]
    chemin.moveTo(*sommets[0])
    for x, y in sommets[1:]:
        chemin.lineTo(x, y)
    chemin.closeSubpath()
 
 
def ruban(chemin, points, largeurs):
    """Ajoute un ruban effilé : une ligne brisée à laquelle on donne une
    épaisseur variable. C'est ce qui fabrique les pattes (larges à
    l'attache, fines au bout) sans un seul trait de crayon.
 
    points   : [(x, y), ...]     le trajet de la patte
    largeurs : [l0, l1, ...]     une largeur par point
    """
    gauche, droite = [], []
    for i, (x, y) in enumerate(points):
        avant = points[max(i - 1, 0)]
        apres = points[min(i + 1, len(points) - 1)]
        dx, dy = apres[0] - avant[0], apres[1] - avant[1]
        longueur = math.hypot(dx, dy) or 1.0
        nx, ny = -dy / longueur, dx / longueur      # la normale
        demi = largeurs[i] / 2
        gauche.append((x + nx * demi, y + ny * demi))
        droite.append((x - nx * demi, y - ny * demi))
    polygone(chemin, gauche + droite[::-1])
 
 
# ==========================================================================
#  LA FOURMI  —  c'est ici que tu règles
# ==========================================================================
 
# Chaque liste est un polygone : un sommet par ligne.
ABDOMEN = [
    (-9.1,  0.0),    # pointe arrière
    (-8.9, -0.9),
    (-8.3, -1.7),
    (-6.9, -2.1),
    (-5.0, -2.0),
    (-3.8, -1.2),
    (-3.5,  -0.35),    # attache vers la taille

]
 
THORAX = [
    (-3.4, -0.35),
    (-3.2, -0.8),
    (-2.8, -1.0),
    (-2.5, -0.9),
    (-2.0, -0.7),
    (-1.3, -0.6),
    (-0.6, -0.7),
    ( 0.0, -0.9),
    ( 0.7, -1.1),
    ( 1.3, -1.3),
    ( 1.6, -1.25),
    ( 1.8, -1.0),
    ( 2.1,  -0.6),
]
 
TETE = [
    ( 2.3, -0.6),
    ( 2.0, -1.1),
    ( 1.85, -1.3),
    ( 2.0, -1.6),
    ( 2.6, -1.95),
    ( 3.15, -2.1),
    ( 4.15, -2.1),
    ( 5, -1.9),
    ( 5.7, -1.7),
    ( 6.2, -1.4),
    ( 6.7, -0.8),
    ( 6.9, -0.4),
    ( 6.4, -0.8),
    ( 6.2, -0.0),
]
 
# Les jonctions entre les trois parties : la taille fine (pétiole) et le cou.
PETIOLE = [
    (-3.5, -0.35),
    (-3.4, -0.35),
]
 
COU = [
    (2.1, -0.6),
    (2.3, -0.6),
]

# L'oeil : un seul polygone, celui du HAUT (y negatif). L'oeil du bas est
# fabrique automatiquement par symetrie(), comme les pattes.
# Il est dessine APRES le corps, avec COULEUR_YEUX, donc il doit se poser
# SUR la tete : garde ses points a l'interieur du contour TETE.
OEIL = [
    (3.5, -1.8),
    (4.2, -1.8),
    (4.35, -1.75),
    (4.5, -1.50),
    (4.35, -1.25),
    (4.2, -1.20),
    (3.5, -1.20),
    (3.3, -1.5),
]
 
# Les pattes : trajet + largeur en chaque point.
# Elles partent dans trois directions différentes — c'est CE détail qui fait
# qu'on lit "fourmi" et pas "peigne". Si tu ne règles qu'une chose, règle ça.
PATTES = [
    ([(-3.0, 0.7), (-3.7, 3.3), (-5.6, 5.3)], [0.5, 0.3, 0.1]),   # arrière
    ([(-1.6, 0.6), (-1.3, 3.9), (-1.8, 5.9)], [0.5, 0.3, 0.1]),   # milieu
    ([( -0.4, 0.4), ( 1.2, 3.3), ( 3.9, 4.7)], [0.5, 0.3, 0.1]),   # avant
]
 
# Les antennes : coudées, plus fines.
ANTENNES = [
    ([(5.1, -1.4), (5.3, -3.5), (8.9, -5.1)], [0.2, 0.2, 0.05]),
]
 
 
def forme():
    """Renvoie le dessin de la fourmi. C'est tout ce que l'atelier demande."""
    p = QPainterPath()
 
    # Regle de remplissage : sans elle, deux polygones qui se chevauchent se
    # "trouent" mutuellement (une patte creuserait un trou dans le thorax).
    p.setFillRule(Qt.FillRule.WindingFill)
 
    # le corps : chaque piece est refermee par son propre reflet
    for demi in (ABDOMEN, PETIOLE, THORAX, COU, TETE):
        polygone(p, miroir(demi))
 
    for trajet, largeurs in PATTES + ANTENNES:
        ruban(p, trajet, largeurs)
        ruban(p, symetrie(trajet), largeurs)
 
    return p
 
 
def forme_yeux():
    """Renvoie le dessin des DEUX yeux, dans un chemin separe.
 
    Pourquoi separe : l'atelier peint un chemin avec une seule couleur. Si les
    yeux etaient dans le meme chemin que le corps, ils prendraient la couleur
    du corps (ou le corps prendrait celle des yeux). Deux chemins = deux
    couleurs.
 
    Pas de miroir() ici : miroir() referme UNE piece posee sur l'axe, alors
    qu'un oeil est une piece entiere posee a cote de l'axe. C'est symetrie()
    qui donne l'oeil de l'autre cote, exactement comme pour les pattes.
    """
    p = QPainterPath()
    p.setFillRule(Qt.FillRule.WindingFill)
    polygone(p, OEIL)
    polygone(p, symetrie(OEIL))
    return p