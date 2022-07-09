import vtk 


import numpy as np
import matplotlib.pyplot as plt

def _(lst):
    return [np.array(p) for p in lst]

frame = _([
    [-214.329, 204.022, -1448.263],
    [-163.324, 133.213, -1489.936],
    [-126.552, 201.482, -1630.177],
    [-114.78, 207.65, -1471.794],
    [-115.392, 163.262, -1489.678],
    [-105.579, 172.423, -1356.773],
    [-76.388, 102.576, -1454.241],
    [-32.654, 77.826, -1431.705],
    [-29.226, 104.72, -1229.31],
    [-39.929, 146.105, -1739.038],
    [-34.633, 166.222, -1447.229]
])

patterns = [
    _([
        [-0.14, -0.0, 0.0],
        [0.12, -63.08, 29.42],
        [0.12, 61.77, 109.39],
        [-0.1, 0.0, 165.65]
    ])
]


def findClosestPoint(frame, point):
    min_dist = 1000000
    min_index = 0
    for i in range(len(frame)):
        dist = np.linalg.norm(frame[i] - point)
        if dist < min_dist:
            min_dist = dist
            min_index = i
    return min_index


for k, pattern in enumerate(patterns):
    print(pattern)
    sourcePoints = vtk.vtkPoints()
    sourceVertices = vtk.vtkCellArray()
    for point in pattern:
        id = sourcePoints.InsertNextPoint(point[0], point[1], point[2])
        sourceVertices.InsertNextCell(1)
        sourceVertices.InsertCellPoint(id)
    source = vtk.vtkPolyData()
    source.SetPoints(sourcePoints)
    source.SetVerts(sourceVertices)


    targetPoints = vtk.vtkPoints()
    targetVertices = vtk.vtkCellArray()
    for point in frame:
        id = targetPoints.InsertNextPoint(point[0], point[1], point[2])
        targetVertices.InsertNextCell(1)
        targetVertices.InsertCellPoint(id)
    target = vtk.vtkPolyData()
    target.SetPoints(targetPoints)
    target.SetVerts(targetVertices)

    icp = vtk.vtkIterativeClosestPointTransform()
    icp.SetSource(source)
    icp.SetTarget(target)
    icp.StartByMatchingCentroidsOn()
    icp.SetMaximumNumberOfIterations(100)
    icp.GetLandmarkTransform().SetModeToRigidBody()
    icp.Update()

    transformation = icp.GetMatrix()
    print(transformation)

    transform = vtk.vtkTransform()
    transform.SetMatrix(transformation)
    transform.Update()

    mapped_source = vtk.vtkPoints()
    transform.TransformPoints(sourcePoints, mapped_source)

    transformedPoints = [mapped_source.GetPoint(i) for i in range(mapped_source.GetNumberOfPoints())]

    for point in transformedPoints:
        print(point)
    print()

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    
    ax.set_title('Tool number ' + str(k))
    for i, point in enumerate(frame):
        if k == 2 and i in (6, 7, 10):
            ax.scatter(point[0], point[1], point[2], c='g', marker='o')
        elif k == 4 and i in (0, 1, 3, 4):
            ax.scatter(point[0], point[1], point[2], c='g', marker='o')
        else:
            ax.scatter(point[0], point[1], point[2], c='b', marker='o')
    
    for point in transformedPoints:
        ax.scatter(point[0], point[1], point[2], c='r', marker='o')
    


    for point in transformedPoints:
        distances = [ np.linalg.norm(point - frame[i]) for i in range(len(frame)) ]
        min_index = distances.index(min(distances))
        ax.plot([point[0], frame[min_index][0]], [point[1], frame[min_index][1]], [point[2], frame[min_index][2]], c='r')


    plt.show()
    
plt.show()
