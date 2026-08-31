import importlib
import sys
import time
import traceback
 
import fourmi_forme
 
# Marche avec PySide6, PyQt6 ou PyQt5 : on prend la premiere installee.
# (Une seule est chargee : melanger deux liaisons Qt fait planter le programme.)
try:
    from PySide6.QtCore import QFileSystemWatcher, QPointF, Qt, QTimer
    from PySide6.QtGui import QBrush, QColor, QFont, QPainter, QPen
    from PySide6.QtWidgets import QApplication, QWidget
    LIAISON = "PySide6"
except ModuleNotFoundError:
    try:
        from PyQt6.QtCore import QFileSystemWatcher, QPointF, Qt, QTimer
        from PyQt6.QtGui import QBrush, QColor, QFont, QPainter, QPen
        from PyQt6.QtWidgets import QApplication, QWidget
        LIAISON = "PyQt6"
    except ModuleNotFoundError:
        from PyQt5.QtCore import QFileSystemWatcher, QPointF, Qt, QTimer
        from PyQt5.QtGui import QBrush, QColor, QFont, QPainter, QPen
        from PyQt5.QtWidgets import QApplication, QWidget
        LIAISON = "PyQt5"
 
 
def position_souris(evenement):
    """Qt6 dit .position(), Qt5 dit .localPos(). On accepte les deux."""
    if hasattr(evenement, "position"):
        return evenement.position()
    return evenement.localPos()
 
FICHIER = "fourmi_forme.py"
 
 
class Atelier(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Atelier fourmi")
        self.resize(1000, 680)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
 
        self.zoom = 26.0
        self.pan = QPointF(0, 0)
        self.angle = 0.0
        self.grille = True
        self.contour = False
        self.cadre = False
        self.anime = False
        self.erreur = ""
        self.recharge = ""
        self._depart_glisse = None
 
        self.chemin = None
        self.chemin_yeux = None
        self.charger()
 
        # surveille le fichier de forme et recharge à chaque sauvegarde
        self.veilleur = QFileSystemWatcher([FICHIER])
        self.veilleur.fileChanged.connect(self._fichier_modifie)
 
        self.horloge = QTimer(self)
        self.horloge.timeout.connect(self._tic)
        self.horloge.start(16)
 
    # ------------------------------------------------------------------
    # rechargement à chaud
    # ------------------------------------------------------------------
    def charger(self):
        try:
            importlib.reload(fourmi_forme)
            self.chemin = fourmi_forme.forme()
            # les yeux vivent dans leur propre chemin : c'est ce qui permet de
            # les peindre d'une autre couleur que le corps.
            fabrique_yeux = getattr(fourmi_forme, "forme_yeux", None)
            self.chemin_yeux = fabrique_yeux() if fabrique_yeux else None
            self.erreur = ""
            self.recharge = time.strftime("%H:%M:%S")
        except Exception:
            # on garde le dernier dessin valide et on affiche l'erreur
            self.erreur = traceback.format_exc(limit=3)
        self.update()
 
    def _fichier_modifie(self, chemin):
        # beaucoup d'éditeurs remplacent le fichier au lieu de l'écrire :
        # le veilleur perd sa cible, il faut la remettre.
        QTimer.singleShot(80, lambda: (self.veilleur.addPath(FICHIER),
                                       self.charger()))
 
    def _tic(self):
        if self.anime:
            self.angle = (self.angle + 0.8) % 360
            self.update()
 
    # ------------------------------------------------------------------
    # dessin
    # ------------------------------------------------------------------
    def paintEvent(self, _):
        fond = getattr(fourmi_forme, "FOND", QColor("#e8e2d5"))
        qp = QPainter(self)
        qp.fillRect(self.rect(), fond)
        qp.setRenderHint(QPainter.RenderHint.Antialiasing)
 
        cx = self.width() / 2 + self.pan.x()
        cy = self.height() / 2 + self.pan.y()
 
        if self.grille:
            self._dessiner_grille(qp, cx, cy)
 
        # la fourmi, en grand
        qp.save()
        qp.translate(cx, cy)
        qp.scale(self.zoom, self.zoom)
        qp.rotate(self.angle)
        self._dessiner_fourmi(qp)
        self._dessiner_yeux(qp)
        if self.cadre and self.chemin is not None:
            qp.setBrush(Qt.BrushStyle.NoBrush)
            qp.setPen(QPen(QColor("#c0392b"), 1 / self.zoom, Qt.PenStyle.DashLine))
            qp.drawRect(self.chemin.boundingRect())
        qp.restore()
 
        self._dessiner_vignettes(qp)
        self._dessiner_hud(qp)
 
    def _dessiner_fourmi(self, qp):
        if self.chemin is None:
            return
        couleur = getattr(fourmi_forme, "COULEUR", QColor("#33291f"))
        ep = getattr(fourmi_forme, "EPAISSEUR", 0.7)
        if self.contour:
            qp.setBrush(Qt.BrushStyle.NoBrush)
            qp.setPen(QPen(couleur, ep * 0.6))
        else:
            qp.setBrush(QBrush(couleur))
            qp.setPen(QPen(couleur, ep))
        qp.drawPath(self.chemin)

    def _dessiner_yeux(self, qp):
        """Les yeux, par-dessus la tete, avec LEUR couleur a eux."""
        if self.chemin_yeux is None:
            return
        couleur = getattr(fourmi_forme, "COULEUR_YEUX", QColor("#000000"))
        ep = getattr(fourmi_forme, "EPAISSEUR", 0.7)
        if self.contour:
            qp.setBrush(Qt.BrushStyle.NoBrush)
            qp.setPen(QPen(couleur, ep * 0.6))
        else:
            qp.setBrush(QBrush(couleur))
            qp.setPen(QPen(couleur, ep))
        qp.drawPath(self.chemin_yeux)
 
    def _dessiner_grille(self, qp, cx, cy):
        """Grille en unités de la fourmi : 1 trait par unité, marqué tous les 5."""
        ux = self.width() / self.zoom
        uy = self.height() / self.zoom
        umax = int(max(ux, uy) / 2) + 2
 
        fin = QPen(QColor(0, 0, 0, 26), 0)
        gros = QPen(QColor(0, 0, 0, 55), 0)
        axe = QPen(QColor(0, 0, 0, 110), 0)
 
        for u in range(-umax, umax + 1):
            qp.setPen(axe if u == 0 else (gros if u % 5 == 0 else fin))
            x = cx + u * self.zoom
            y = cy + u * self.zoom
            qp.drawLine(int(x), 0, int(x), self.height())
            qp.drawLine(0, int(y), self.width(), int(y))
 
        # les nombres, tous les 5, en pixels (donc taille constante)
        qp.setPen(QColor(0, 0, 0, 130))
        qp.setFont(QFont("monospace", 8))
        for u in range(-(umax // 5) * 5, umax + 1, 5):
            if u == 0:
                continue
            qp.drawText(int(cx + u * self.zoom) + 2, int(cy) - 3, str(u))
            qp.drawText(int(cx) + 4, int(cy + u * self.zoom) - 3, str(u))
 
    def _dessiner_vignettes(self, qp):
        """La même fourmi aux tailles réelles : c'est là qu'on juge la lisibilité."""
        couleur = getattr(fourmi_forme, "COULEUR", QColor("#33291f"))
        couleur_yeux = getattr(fourmi_forme, "COULEUR_YEUX", QColor("#000000"))
        ep = getattr(fourmi_forme, "EPAISSEUR", 0.7)
        if self.chemin is None:
            return
        x = self.width() - 150
        y = 30
        qp.setPen(QColor(0, 0, 0, 120))
        qp.setFont(QFont("monospace", 8))
        qp.drawText(x - 40, y - 12, "tailles réelles")
        for facteur in (1, 2, 4):
            qp.save()
            qp.translate(x, y)
            qp.scale(facteur, facteur)
            qp.setBrush(QBrush(couleur))
            qp.setPen(QPen(couleur, ep))
            qp.drawPath(self.chemin)
            if self.chemin_yeux is not None:
                qp.setBrush(QBrush(couleur_yeux))
                qp.setPen(QPen(couleur_yeux, ep))
                qp.drawPath(self.chemin_yeux)
            qp.restore()
            qp.setPen(QColor(0, 0, 0, 120))
            qp.drawText(x + 45, y + 3, f"x{facteur}")
            y += 16 * facteur
 
    def _dessiner_hud(self, qp):
        qp.setFont(QFont("monospace", 9))
        aide = ("G grille   C contour   B cadre   A rotation   "
                "molette zoom   R remise à zéro")
        qp.setPen(QColor(0, 0, 0, 140))
        qp.drawText(12, self.height() - 14, aide)
 
        if self.erreur:
            qp.setPen(QColor("#c0392b"))
            qp.drawText(12, 22, "ERREUR dans fourmi_forme.py "
                                "(dernier dessin valide conservé) :")
            for i, ligne in enumerate(self.erreur.strip().splitlines()[-6:]):
                qp.drawText(12, 40 + i * 14, ligne[:120])
        else:
            qp.setPen(QColor(0, 0, 0, 120))
            qp.drawText(12, 22, f"rechargé à {self.recharge}   "
                                f"zoom x{self.zoom:.0f}   [{LIAISON}]")
 
    # ------------------------------------------------------------------
    # interactions
    # ------------------------------------------------------------------
    def wheelEvent(self, e):
        self.zoom *= 1.15 if e.angleDelta().y() > 0 else 1 / 1.15
        self.zoom = min(max(self.zoom, 2.0), 400.0)
        self.update()
 
    def mousePressEvent(self, e):
        self._depart_glisse = position_souris(e) - self.pan
 
    def mouseMoveEvent(self, e):
        if self._depart_glisse is not None:
            self.pan = position_souris(e) - self._depart_glisse
            self.update()
 
    def mouseReleaseEvent(self, _):
        self._depart_glisse = None
 
    def keyPressEvent(self, e):
        k = e.key()
        if k == Qt.Key.Key_G:
            self.grille = not self.grille
        elif k == Qt.Key.Key_C:
            self.contour = not self.contour
        elif k == Qt.Key.Key_B:
            self.cadre = not self.cadre
        elif k == Qt.Key.Key_A:
            self.anime = not self.anime
            if not self.anime:
                self.angle = 0.0
        elif k == Qt.Key.Key_R:
            self.zoom, self.pan, self.angle = 26.0, QPointF(0, 0), 0.0
        elif k == Qt.Key.Key_Escape:
            self.close()
        self.update()
 
 
def main():
    app = QApplication(sys.argv)
    atelier = Atelier()
    atelier.show()
    sys.exit(app.exec())
 
 
if __name__ == "__main__":
    main()