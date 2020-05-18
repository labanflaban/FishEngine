import sys
import pymel.core as pm
import pymel.core.datatypes as dt
import os.path
from PySide2 import QtWidgets, QtCore

class Interface(QtWidgets.QDialog):
    def __init__(self, parent = None):
        super(Interface, self).__init__(parent)
        
        self.selectedPath = ''
        
        self.fileNamePath = QtWidgets.QLineEdit()
        
        
        self.fileNameTitle = QtWidgets.QLabel("Filename")
        self.fileInformation = QtWidgets.QLabel("To export the mesh info you need to select the meshes")        
        self.fileBrowse = QtWidgets.QPushButton("Browse")

        self.fileNamePath.textChanged.connect(self.changeText)
        self.fileBrowse.clicked.connect(self.finalStep)

        self.formLayout = QtWidgets.QFormLayout()
        self.formLayout.addWidget(self.fileNameTitle)
        self.formLayout.addWidget(self.fileInformation)
        self.formLayout.addWidget(self.fileNamePath)
        self.formLayout.addWidget(self.fileBrowse)

        
        self.setLayout(self.formLayout)

    def changeText(self,nameforfile):
        self.selectedPath = nameforfile
        print(self.selectedPath)
    
  
    '''MAINCODE----------------------------'''
    
    def finalStep(self):

		meshList = []
		meshList = pm.ls(selection = True)		
		meshName = []
		translationVec = []
		rotationVec = []
		scaleVec = []
		boolList = []
		tagList = []

		self.filePath = pm.fileDialog2(startingDirectory = self.selectedPath, fileFilter="(*.level")
		savePath = str(self.filePath)

		splitFileName = str(self.filePath)
		splitFileName = splitFileName.split("'")
		splitFileName = splitFileName[1]
		savePath = splitFileName

		saveFP  = os.path.join(savePath)
		saveFP = open(saveFP,'w')
		
		if len(meshList) > 0:
			print 'yes'
		for i, mesh in enumerate(meshList):
					
			meshName.append(mesh.name())
			translationVec.append(mesh.getTranslation())
			rotationVec.append(mesh.getRotation())
			scaleVec.append(mesh.getScale())
			if meshName[i].find('background') != -1:
				boolList.append(1)
				tagList.append('background')
			else:
				boolList.append(0)
			
			if meshName[i].find('platform') != -1:
				tagList.append('platform')
				
			elif(meshName[i].find('enemy') != -1):
				tagList.append('enemy')
				
			elif(meshName[i].find('character') != -1):
				tagList.append('character')
				
			elif(meshName[i].find('ground') != -1):
				tagList.append('ground')

			print (meshName[i])
			print (translationVec[i])
			print (rotationVec[i])
			print (scaleVec[i])
			print boolList[i]
			print tagList[i]
			
		# write loop
		for i, mesh in enumerate(meshList):
			saveFP.write('%s '%(meshName[i]))
			for j in range(3):
				saveFP.write('%s '%(translationVec[i][j]))
			for j in range(3):
				saveFP.write('%s '%(rotationVec[i][j]))
			for j in range(3):
				saveFP.write('%s '%(scaleVec[i][j]))
			saveFP.write('%s '%(boolList[i]))
			saveFP.write('%s '%(tagList[i]))
			saveFP.write('\r\n')
		
		saveFP.close()


app = QtWidgets.QApplication.instance()
interface = Interface()
interface.resize(300, 200)
interface.show()   