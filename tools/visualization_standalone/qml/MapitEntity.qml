import Qt3D.Core 2.0 as Q3D
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQml 2.2

import fhac.upns 1.0 as UPNS

Q3D.Entity {
    //property alias transformMat: theMeshTransform.matrix
    property alias currentEntitydata : edrender.entitydata
    //property var currentEntitydata
    property alias currentTransform : currentEntitydataTransform
    property var mainCameratmp
    property var scene3dtmp
    property Layer layer
    id: pointcloud

    UPNS.EntitydataTransform {
        id: currentEntitydataTransform
        checkout: checkout
        path: edrender.entitydata + ".tf"
        mustExist: false
    }
    property ObjectPicker picker: ObjectPicker {
        onClicked: console.log("Clicked pcd", pick.distance, pick.triangleIndex)
    }
    property var meshTransform: Q3D.Transform {
            id: theMeshTransform
            matrix: currentEntitydataTransform.matrix
            //property real userAngle: -90.0
            //scale: 10
            //rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), userAngle)

        }
    property GeometryRenderer customMesh: UPNS.EntitydataRenderer {
        id: edrender
        }
    property Material materialPoint: Material {
        effect: Effect {
            techniques: Technique {
                renderPasses: RenderPass {
                    shaderProgram: ShaderProgram {
                        //vertexShaderCode: loadSource("qrc:/shader/pointcloud.vert")
                        //fragmentShaderCode: loadSource("qrc:/shader/pointcloud.frag")
                        vertexShaderCode: loadSource("qrc:/shader/surfel.vert")
                        fragmentShaderCode: loadSource("qrc:/shader/surfel.frag")
                    }
                }
            }
        }
        parameters: [
            Parameter { name: "pointSize"; value: 0.5 },
            Parameter { name: "fieldOfView"; value: mainCameratmp.fieldOfView },
            Parameter { name: "fieldOfViewVertical"; value: mainCameratmp.fieldOfView/mainCameratmp.aspectRatio },
            Parameter { name: "nearPlane"; value: mainCameratmp.nearPlane },
            Parameter { name: "farPlane"; value: mainCameratmp.farPlane },
            Parameter { name: "width"; value: scene3dtmp.width },
            Parameter { name: "height"; value: scene3dtmp.height }
        ]
    }
//    property var componentsWMesh: [ customMesh, materialPoint, meshTransform, layer, picker ]
//    property var componentsWOMesh: [ materialPoint, meshTransform, layer, picker ]
    components: [ customMesh, materialPoint, meshTransform, layer, picker ] //componentsWOMesh
}
