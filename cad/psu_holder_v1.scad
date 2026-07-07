/*******************************************************************************
TITLE:
  Snap-in-Halter für USB-Netzteilmodul (Amazon B0FR192P69)

BESCHREIBUNG:
  Druckbare Grundplatte mit rechteckigem Rahmen, in den die liegende Platine
  des AC-DC-USB-Netzteils (230 V -> 5 V / 2,1 A, Platine 31 x 37 mm, USB-Buchse
  mittig auf der langen Kante) von oben eingedrückt wird. Zwei flexible
  Schnapp-Finger an den kurzen Enden rasten über die Platinen-Oberkante und
  halten sie zwischen Finger (oben), Auflage-Absatz (unten) und Wänden (seitlich).

  Die 4 Befestigungslöcher liegen AUSSERHALB der Platine -> Clip zuerst testen,
  dann die Platte auf die Sockel (DeviceHolder-Satz 3) im Gehäuse schrauben.

WIE ES HÄLT:
  - unten: umlaufender Auflage-Absatz (seat_h) trägt die Platine.
  - seitlich: die Rahmenwände umschließen die Platine (kann nicht kippen).
  - oben: zwei Schnapp-Finger greifen mit einer Nase (hook_over) über die
    Platinen-Oberkante. Beim Eindrücken weichen die Finger nach außen aus
    (Einführschräge) und schnappen zurück -> Platine sitzt fest.

MONTAGE:
  1. Platine in den Rahmen drücken bis beide Finger einrasten (Clip testen).
  2. Platte mit 4 selbstschneidenden M3-Schrauben auf die Sockel schrauben.
  3. USB-Kabel in die Buchse (Aussparung an der langen Wand).

KOPPLUNG ZUM GEHÄUSE:
  screw_dx / screw_dy MÜSSEN mit DeviceHolder_X_Distance3 / _y_Distance3 im
  Gehäuse ultimatewaterproofcase_v74.scad übereinstimmen (siehe echo unten).

SICHERHEIT:
  Der AC-Eingang der Platine führt 230 V. Die USB-Aussparung liegt an der
  langen Wand; die AC-Kontakte möglichst zur geschlossenen Wand orientieren.

Einheiten: mm. Ursprung: Mitte der Platte, Unterseite bei z = 0.
X = lange (37 mm) Richtung, Y = kurze (31 mm) Richtung.
Druck: Platte flach aufs Bett, kein Support nötig.
*******************************************************************************/

/* [Render] */
$fn = 64;

/* [Platinen-Maße - MESSEN] */
// Lange Kante der Platine (X) - hier liegt mittig die USB-Buchse
pcb_x      = 37;
// Kurze Kante der Platine (Y)
pcb_y      = 31;
// Höchstes Bauteil über der Platine (Trafo) - nur Bauhöhen-Check
mod_height = 16;
// Platinendicke
pcb_thick  = 1.0;
// Spiel rundum im Rahmen (größer = leichter einlegbar)
fit_tol    = 0.9;

/* [USB-Buchse - an der langen Wand, mittig] */
// Breite der USB-A-Buchse entlang der Kante (X)
usb_w        = 13;
// Höhe der Buchse über der Platinenoberkante
usb_h        = 7;
// Versatz der Buchse aus der Mitte entlang X (0 = mittig)
usb_from_mid = 0;
// Auf welcher langen Wand: +1 = +Y, -1 = -Y
usb_side     = 1;

/* [Power-/AC-Anschluss - übersteht die Platine, Aussparung wie USB] */
// Breite der Aussparung entlang der Kante (X)
pwr_w        = 18;
// Höhe über der Platinenoberkante
pwr_h        = 7;
// Versatz aus der Mitte entlang X (negativ = links); Default: links bis Mitte
pwr_from_mid = -9;
// Auf welcher langen Wand: +1 = +Y, -1 = -Y (Default gegenüber USB)
pwr_side     = -1;

/* [Rahmen] */
// Wandstärke des Rahmens
wall = 2.0;
// Höhe des umlaufenden Auflage-Absatzes (Platine liegt hier auf)
// muss die Bauteile auf der Platinen-UNTERSEITE freihalten (hier 2,3 mm + Luft)
seat_h = 3.0;
// Breite des Auflage-Absatzes nach innen
ledge_w = 1.5;
// Rahmenhöhe über der Plattenoberseite
frame_h = 5.0;
// Eckenfreistich im Innenraum (FDM verrundet Innenecken -> Platinenecke frei)
corner_relief = 2.5;

/* [Schnapp-Finger (an den kurzen Enden)] */
// Breite eines Fingers (entlang Y)
finger_w = 10;
// Dicke/Stärke eines Fingers (dünner = weicher/federnder)
finger_th = 1.2;
// Seitliches Spiel um den Finger (Schlitz zum Federn)
finger_gap = 1.5;
// Überstand der Haltenase nach innen (greift über die Platine)
hook_over = 0.8;
// Höhe der Haltenase
hook_h = 1.5;

/* [Platte & Befestigung (Löcher AUSSERHALB der Platine)] */
// Plattendicke
plate_thick = 3.0;
// Materialrand um die Schraublöcher (Ohren)
ear = 5.0;
// Eckenradius der Platte
corner_r = 4.0;
// Loch-Abstand in X (== DeviceHolder_X_Distance3 im Gehäuse!)
// Löcher liegen jenseits der kurzen Enden -> Platte bleibt in Y schmal
screw_dx = 48;
// Loch-Abstand in Y (== DeviceHolder_y_Distance3 im Gehäuse!)
screw_dy = 20;
// Durchgangsloch für M3 selbstschneidend
screw_hole_d = 3.2;
// Senkung Schraubenkopf: Durchmesser
screw_head_d = 6.0;
// Senkung Schraubenkopf: Tiefe
screw_head_h = 2.4;

/* [Berechnet - nicht ändern] */
eps     = 0.05;
in_len  = pcb_x + fit_tol;              // Innenmaß Rahmen X
in_wid  = pcb_y + fit_tol;              // Innenmaß Rahmen Y
out_len = in_len + 2*wall;              // Aussenmaß Rahmen X
out_wid = in_wid + 2*wall;              // Aussenmaß Rahmen Y
plate_x = max(out_len, screw_dx + 2*ear);
plate_y = max(out_wid, screw_dy + 2*ear);
grip_z  = plate_thick + seat_h + pcb_thick;   // z der Platinen-Oberkante
fin_top = grip_z + hook_h;                    // Oberkante der Finger

// ---------------------------------------------------------------- Hilfsformen
module rrect(l, w, h, r) {
    linear_extrude(height=h)
        offset(r=r) offset(delta=-r) square([l, w], center=true);
}

// -------------------------------------------------------------------- Platte
module base_plate() {
    difference() {
        rrect(plate_x, plate_y, plate_thick, corner_r);
        for (sx = [-1, 1], sy = [-1, 1])
            translate([sx*screw_dx/2, sy*screw_dy/2, 0]) {
                translate([0, 0, -eps])
                    cylinder(h=plate_thick+2*eps, d=screw_hole_d);
                translate([0, 0, plate_thick-screw_head_h])
                    cylinder(h=screw_head_h+eps, d=screw_head_d);
            }
    }
}

// -------------------------------------------------------------------- Rahmen
module frame() {
    translate([0, 0, plate_thick]) {
        difference() {
            // Aussenwände
            rrect(out_len, out_wid, frame_h, min(corner_r, wall));
            // Innenraum (oben offen)
            translate([0, 0, -eps])
                linear_extrude(height=frame_h+2*eps)
                    square([in_len, in_wid], center=true);
            // Eckenfreistiche (scharfe Platinenecken passen trotz FDM-Radius)
            for (sx = [-1, 1], sy = [-1, 1])
                translate([sx*in_len/2, sy*in_wid/2, -eps])
                    cylinder(h=frame_h+2*eps, d=corner_relief);
            // USB-Aussparung in der langen (usb_side) Wand, mittig
            translate([usb_from_mid, usb_side*out_wid/2, seat_h + usb_h/2])
                cube([usb_w, 4*wall, (frame_h) + 2*usb_h], center=true);
            // Aussparung für Power-/AC-Anschluss (übersteht die Platine)
            translate([pwr_from_mid, pwr_side*out_wid/2, seat_h + pwr_h/2])
                cube([pwr_w, 4*wall, (frame_h) + 2*pwr_h], center=true);
            // Fenster für die Schnapp-Finger an beiden kurzen Enden
            for (sx = [-1, 1])
                translate([sx*out_len/2, 0, (frame_h)/2])
                    cube([2*wall+2*eps, finger_w+2*finger_gap, frame_h+2*eps],
                         center=true);
        }
        // umlaufender Auflage-Absatz (Ledge)
        difference() {
            translate([0, 0, seat_h/2])
                cube([in_len, in_wid, seat_h], center=true);
            translate([0, 0, seat_h/2])
                cube([in_len-2*ledge_w, in_wid-2*ledge_w, seat_h+2*eps],
                     center=true);
            // Ledge an der USB-Stelle weglassen
            translate([usb_from_mid, usb_side*in_wid/2, seat_h/2])
                cube([usb_w, 2*ledge_w+2*eps, seat_h+2*eps], center=true);
            // Ledge an der Power-Stelle weglassen
            translate([pwr_from_mid, pwr_side*in_wid/2, seat_h/2])
                cube([pwr_w, 2*ledge_w+2*eps, seat_h+2*eps], center=true);
            // Ledge an den Finger-Stellen weglassen -> Finger kann frei federn
            // (sonst versteift der 3-mm-Absatz den Finger bis kurz unter die Nase)
            for (sx = [-1, 1])
                translate([sx*(in_len/2 - ledge_w/2), 0, seat_h/2])
                    cube([ledge_w+2*eps, finger_w+2*finger_gap, seat_h+2*eps],
                         center=true);
        }
    }
}

// ----------------------------------------------------------- Schnapp-Finger
// Finger an einem kurzen Ende: senkrechter Balken an der Wandlinie, oben eine
// Nase, die nach innen über die Platine greift. Unterseite als Rampe.
module snap_finger() {
    xi = in_len/2;                         // Innenfläche der kurzen Wand (+X)
    // Fingerkörper (ersetzt das Wandsegment, federt nach +X)
    translate([xi + finger_th/2, 0, (plate_thick + fin_top)/2])
        cube([finger_th, finger_w, fin_top - plate_thick], center=true);
    // Haltenase mit Rampen-Unterseite (Einführschräge)
    hull() {
        translate([xi - hook_over/2, 0, grip_z + hook_h/2])
            cube([hook_over, finger_w, hook_h], center=true);
        translate([xi, 0, grip_z - hook_h])
            cube([0.02, finger_w, 0.02], center=true);
    }
}

module snap_fingers() {
    snap_finger();                         // +X Ende
    mirror([1, 0, 0]) snap_finger();       // -X Ende
}

// ------------------------------------------------------------------- Aufbau
module holder() {
    color("PaleGreen") base_plate();
    color("LightBlue") frame();
    color("Salmon")    snap_fingers();
}

holder();

// -------------------------------------------------------------------- Ausgabe
echo(str("Rahmen-Innenmass (X x Y): ", in_len, " x ", in_wid, " mm"));
echo(str("Platte (X x Y): ", plate_x, " x ", plate_y, " mm"));
echo(str("Loch-Abstand screw_dx x screw_dy: ", screw_dx, " x ", screw_dy,
         " mm  (== DeviceHolder_X/y_Distance3!)"));
echo(str("Platinen-Oberkante bei z = ", grip_z, " mm (Finger greifen hier)"));
echo(str("Modul-Oberkante ab Plattenunterseite: ",
         plate_thick + seat_h + pcb_thick + mod_height, " mm"));
echo(str("Modul-Oberkante im Gehäuse (Boden z=3, Sockel 4mm): z = ",
         3 + 4 + plate_thick + seat_h + pcb_thick + mod_height, " mm",
         "  (Bodenhälfte 30mm, Deckel-Innen 37mm)"));
