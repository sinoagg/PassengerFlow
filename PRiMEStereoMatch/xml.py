#coding=utf-8
#from xml.etree import ElementTree

#from xml.etree import ElementTree as ET

#root = ET.Element(--<opencv_storage>)

#son=ET.SubElement(  --<images>)

#ET.SubElement('haha',{'bb':'fhfhf'})


#son=ET.SubElement(    </images>)
#root = ET.Element(  </opencv_storage>)
#		tree.write('aha.xml')


		#=========================================
#!/usr/bin/env python
import os, re
from xml.etree import ElementTree as etree
Locale_Path = "./locale.txt"
class xmlExtractor(object):
  def __init__(self):
    pass
  def __call__(self, filepath):
    retDict = {}
    f = open(filepath, 'r')
    Line = len(open(filepath, 'r').readlines())
    retDict['Line'] = Line
    tree = etree.parse(f)
    root = tree.find("ResItem")
    Id = root.get("ID")
    retDict['Title'] = Id
    resItemCnt = len(list(root.findall("ResItem"))) + 1
    retDict['ResItemCount'] = resItemCnt
    retDict['ChineseTip'] = 'None'
    for child in root:
      attrDict = child.attrib
      keyword = "Name"
      if(keyword in attrDict.keys() and attrDict['Name'] == "Caption"):
        if len(child.attrib['Value']) > 1:
          if child.attrib['Value'][0] == '~':
            title = child.attrib['Value'][1:]
          else:
            title = child.attrib['Value'][0:]
          #print(title)
          chs = open(Locale_Path).read()
          pattern = '<String id="' + title + '">[^>]+>'
          m = re.search(pattern, chs)
          if m != None:
            realTitle = re.sub('<[^>]+>', '', m.group(0))
            retDict['ChineseTip'] = realTitle
    f.close()
    return retDict
if __name__ == "__main__":
  fo = xmlExtractor()
  d = fo('./ActivateAccount.xml')
  print(d)
