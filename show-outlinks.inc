/***********************************************************************************
 Licensed Materials - Property of IBM -  You may copy, modify, and distribute 
 these samples, or their modifications, in any form, internally or as part of 
 your application or related documentation. These samples have not been tested 
 under all conditions and are provided to you by IBM without obligation of 
 support of any kind.  IBM PROVIDES THESE SAMPLES  "AS IS", SUBJECT TO ANY 
 STATUTORY WARRANTIES THAT CANNOT BE EXCLUDED. IBM MAKES NO WARRANTIES 
 OR CONDITIONS, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, 
 THE IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A 
 PARTICULAR PURPOSE, AND NON-INFRINGEMENT REGARDING THESE SAMPLES OR 
 TECHNICAL SUPPORT, IF ANY.

 © Copyright IBM Corporation 2009,2014. All Rights Reserved.

 U.S. Government Users Restricted Rights: Use, duplication or disclosure restricted 
 by GSA ADP Schedule Contract with IBM Corp.                                        

***********************************************************************************/
/*
    Generic code for outlinks traceability columns
    displays the linked objects sorted by their PUID

    void  showOutlinks(Object o, string rel, bool chapter, ole, col, char sep,
                       string attrs[], labels[], bool disp_null, int attr_size, string sAttrSeparator,
                       bool bDisplayText, bDisplayChildren, bDisplayModule)

    parameters:

        Object o        current object
        string rel      name of the relationship to filter. May be absolute ("/proj/satisfies") or relative ("satisfies")
        bool   chapter  to display or not the heading number
        bool   ole      to display or not OLE graphics of object text
        bool   col      to display or not red color for RCM working objects
        char   sep      to display or not a separator between objects, null to not display the separator

        string attrs[]          attributes to display
        string labels[]         labels to display (before the value of the attributes of the previous parameter)
        bool   disp_null        true if you want to display empty attributes
        int    attr_size        to give a fix length to the attribute label, right aligment if negative, left aligment if positive
        string sAttrSeparator   separator inserted between each attribute

        bool   bDisplayText     if false, do not display the object text of the object
        bool   bDisplayChildren if false, do not display the text of the descendants
        bool   bDisplayModule   if false, do not display the name of the module

    the layout of the display is
        
        module (version) §chapter [PUID version]
        object_text
        child_object_text
            attr_label : attr_value
        ----------------------------------------------------
            
    usage

        Example 1

            string attrs[] = {"IE Req Status", "IE Req Flexibility", "IE Req Priority"}
            string labels[]= {"Status",        "Flexibility",        "Priority"}
            showOutlinks (obj, "satisfies", true, false, true, '_', attrs, labels, false, -60, "\n", true, true, true)

        Example 2

            string attrs[] = {}
            string labels[]= {}
            showOutlinks (obj, "refers to", true, false, true, '_', attrs, labels, false, 0, ", ", true, true, true)

*/

/*

Date		Qui				Quoi
17/04/2012	Sopra Group		MYTIC V2.5 : 	Ano 1424 (FT 42) : Ajustement pour afficher tout le temps le chemin du module
29/09/2014	Sopra Group		MYTIC 2.9.0 : 	correction du test sur l'affichage des informations des fils
18/08/2015	N. Deslandres	MYTIC 2.9.0 :	FT 726 :  pas d'encoding UTF8 et espace en trop lorsque l'objet lié est dans l'état In working
 * HISTORIQUE
 * VERSION : 3.3 : FA : 931 : 20/08/2021 : Remplacement de sizeOf par length dans la fonction getChapter (intégration du script de N. Deslandres)
 * FIN-HISTORIQUE 					
*/

pragma runLim, 0
// FT 726 début
pragma encoding, "UTF-8" 
// FT 726 fin

string getChapter(Object o)
{
    int offset, lngth

    string sel = number(o)
    string temp = number(o)
    string tempo = ".0-"

    if (findRichText(sel , tempo , offset, lngth, true))
    {

        int str_end = length(temp)
        str_end = str_end - offset
        temp = cutRichText(sel, offset,str_end )

        return "§" temp
    } 
    else 
    {
        tempo = "-"
        if (findRichText(sel , tempo , offset, lngth, true))
        {
            int str_end = length(temp)
            str_end = str_end - offset
            temp = cutRichText(sel, offset,str_end )

            return "§" temp
        }
        else 
        {
            return "{\\b §" temp "}"
        }
    }
}

void display_composite(Object r,Buffer b,Buffer bt,bool ole)
{
    Object o
    for o in r do
    {
        setempty bt
        if (ole) bt = richTextWithOle o."Object Text"
        else bt = richText o."Object Text"
        if (length(bt) > 0)
        {
            b += '\n'
            combine(b,bt,0)
        }
        display_composite(o,b,bt,ole)
    }
}


void showOutlinks (Object o, string rel, bool chapter, ole, col, char sep,
                   string attrs[], labels[], bool disp_null, int attr_size, string sAttrSeparator,
                   bool bDisplayText, bDisplayChildren, bDisplayModule) 
{
    Link l
    string otherModName
    Module otherMod = null
    Module linkMod = null
    ModuleVersion otherVersion = null
    Object othero
    string disp,s,chap
    int count
    Module lm
    string lname
    Buffer b = create()
    Buffer bt = create()
    Skip sk = createString()
    Buffer bb = create()
    Buffer bufKey = create()

    if (isBatch)
    {
        // some features are not available in batch mode
        sep = null
        attr_size = 0
    }
    int nc = 0
    DBE can = null
	if (! null sep || attr_size != 0)
		can = getCanvas()
		
    int na = 0
    if (! null attrs)
        na = sizeof(attrs)
		
    if (! null can && ! null sep)
    {
        int wc = width can
        int ws = width(can,sep "")
        if (ws > 0)
            nc = wc/ws
    }
    
    for l in all(o->"*") do 
    {
        lm= module(l)
        if (rel[0] == '/')
            lname=fullName(lm)
        else
            lname=name(lm)
        if (lname != rel) continue

        otherVersion = targetVersion l
        if (null module otherVersion) continue
        if (isDeleted module otherVersion) continue
        otherModName = fullName(otherVersion)
        othero = target l
        if (null othero)
        {
            otherMod=read(otherModName,false,true)
            if (isBaseline(otherVersion)) {
                otherMod = load(otherMod,baseline(otherVersion),false)
            }
        }

        othero = target l
        if (null othero) continue
        if (isDeleted othero) continue
        otherMod = module othero

        if (bDisplayModule)
        {
            b = "\\pard \\ul "
            if (getParentFolder(otherMod) == getParentFolder(current Module)) 
            {
            	/* Debut ano 1424 */
            	b += fullName(otherMod)
           		//b += name(otherMod)
           		/* Fin ano 1424 */
            } 
            else 
            {
               b += fullName(otherMod)
            }
            if (isBaseline(otherVersion)) 
            {
               b += " ("
               b += versionString(otherVersion)
               b += ")"
            }
            b += "\\ul0  "
        }
        if (chapter)
        {
            b += getChapter(othero)
            b += ' '
        }
        bool rcm = false
        bool work = false
        if (probeAttr_RMF(otherMod,"RCM Under Control")=="True") 
        {
            rcm = true
            if (probeAttr_RMF(othero,"RCM In Working") == "True")
                work = true
        }

        s = probeAttr_RMF(othero,"IE PUID")
        bufKey = s
        bufKey += "-"
        bufKey += othero."Absolute Number"""

        if (col && work)
        {
            // FT 726 b += "{{\\colortbl ;\\red255\\green0\\blue0;} "
            b += "{{\\colortbl ;\\red255\\green0\\blue0;}"
            b +=  "{\\cf1 "
        }
        b += "{\\b ["
        b += s

        if (rcm)
        {
            string str_version = probeAttr_RMF(othero, "RCM Version")
            if (str_version != "") 
            {
                b += " (v."
                b += str_version
                b += ")"
            }
            
            if (work && ! col)
                b += " w"
        }
        b += "]}"
        if (work && col)
            b += "}}"

        if (bDisplayText)
        {
            b += " \\par \\pard "
            if (ole) bt = richTextWithOle othero."Object Text"
            else bt = richText othero."Object Text"
            combine(b,bt,0)
        }
        
        if (bDisplayChildren) // Sopra
            display_composite(othero,b,bt,ole)

        int nl = 0
        if (! null labels)
            nl = sizeof(labels)
        if (nl > 0 && nl != na)
            nl = 0
        
        int ia
        for (ia=0;ia<na;ia++)
        {
            string an = attrs[ia]
            string lab = null
            if (nl > 0)
                lab = labels[ia]
            
            string v = probeAttr_RMF(othero,an)
            if (null v && !disp_null)
                continue
                
            if (attr_size != 0 && !null can)
            {
                if (attr_size > 0)
                {
                    bb = lab
                    while (true)
                    { 
                        bb += " "
                        int w = width(can,tempStringOf bb)
                        if (w >= attr_size)
                            break
                    }
                }
                else
                {
                    bb = lab
                    int n = 0
                    while (true) 
                    { 
                        bb += " "
                        n++
                        int w = width(can,tempStringOf bb)
                        if (w >= -attr_size)
                            break
                    }
                    bb = ""
                    while (n > 0) {bb += " "; n--}
                    bb += lab
                }
            }
            else
                bb = lab

            if (ia == 0)
                b += " \\par \\pard "
            else
                b += sAttrSeparator

            b += "\\b "
            b += bb
            b += "\\b0 "
            if (! null lab)
                b += " : "

            b += v
        }
        
        while (! put(sk, stringOf(bufKey), stringOf(b)))
            bufKey += " "
        setempty b
        setempty bufKey
    }

    delete bb
    delete bt
    delete bufKey

    Buffer ln = null
    if (! null sep)
    {
        ln = create()
        if (nc <= 0)
            nc = 80
        int i
        for (i=0;i<nc;i++) ln += sep
    }

    int cl = 0
    setempty b
    for s in sk do
    {
        if (cl++ > 0)
        {
            if (! null sep)
            {
                b += " \\par \\pard "
                combine(b,ln,0)
            }
            b += " \\par "
        }
        b += s
    }
	
	if (!null b) { // Sopra
	    if (col)
	        displayRichWithColor tempStringOf b
	    else
	        displayRich tempStringOf b
	    delete b
	}

    delete sk
    if (! null ln)
        delete ln
}
