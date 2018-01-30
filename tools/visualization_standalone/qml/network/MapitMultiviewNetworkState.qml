import QtQuick 2.9
import QtWebSockets 1.1
import QtQml.Models 2.3

// Every peer has one copy of this.
// the own peer state is not included here
// peer states are "stripped": no "ident" is contained

Item {
    id: root
    property var peerToPeerState: ({})
    readonly property ObjectModel realtimeObjects: ObjectModel {}
    property var visibleEntityInfos: ([])
    readonly property ListModel visibleEntityInfosList: ListModel {} //TODO: NodeInstantiator wants ListModel and will crash when using ObjectModel. So two models are used at the moment. Change that when bug is fixed in Qt3D.

    property string repositoryUrl
    //property int repositoryPort
    property string checkoutName

    signal peerJoined(var peer)
    signal peerUpdated(var peer)
    signal peerLeft(var peerIdent)
    signal realtimeObjectAdded(var rto)
    signal realtimeObjectUpdated(var rto)
    signal realtimeObjectRemoved(var rtoIdent)
    signal visibleObjectAdded(var rto)
    signal visibleObjectUpdated(var rto)
    signal visibleObjectRemoved(var rtoIdent)
    function worldUpdated(world) {
        var hostPeer
        // extract add/update/delete
        var newPeers =[] // no old peer for new peer in world. CREATE
        var foundPeers = [] // old peer is found in world. UPDATE
        var missingPeers = [] // old peer is misssng in world. DELETE
        var hashIdentToNewState = {}
        for( var p in peerToPeerState) {
            missingPeers.push(p)
        }
        for( var k=0 ; k < world.length ; ++k) {
            if(world[k].isHost) hostPeer = world[k]
            var worldPeerIdent = world[k].ident
            hashIdentToNewState[worldPeerIdent] = world[k]
            var found = false
            var i = missingPeers.length
            while (i--) {
                // use this strange loop-construct to make manipulation of array "missingPeers" possible while iterating
                if(worldPeerIdent === missingPeers[i]) {
                    missingPeers.splice(i, 1)
                    foundPeers.push(hashIdentToNewState[worldPeerIdent])
                    found = true
                    break
                }
            }
            if( !found ) {
                newPeers.push(hashIdentToNewState[worldPeerIdent])
            }
        }
        //console.log("DBG: peers fin, new: " + newPeers.length + ", found: " + foundPeers.length + ", miss: " + missingPeers.length)
        // RealtimeObjects

        //Do not do the following, in order to have a full "missingRtos" list!
//        // Delete all Objects from deleted peerState
//        var rto = realtimeObjects.length
//        while (rto--) {
//            for( var m=0 ; m < missingPeers.length ; ++m) {
//                var missingPeer = missingPeers[m]
//                if(realtimeObjects[rto].peerOwner === missingPeer) {
//                    realtimeObjects.splice(rto, 1)
//                }
//            }
//        }

        // Update RTOs (use foundPeers for that)
        var newRtos =[] // CREATE
        var foundRtos = [] // UPDATE
        var foundRtosByIdent = {} // UPDATE
        var missingRtos = [] // DELETE
        var missingRtosModelIdx = [] // DELETE
        for( var allRtoI=0 ; allRtoI < realtimeObjects.count ; ++allRtoI) {
            var theRto = realtimeObjects.get(allRtoI)
            missingRtos.push(theRto.ident)
            missingRtosModelIdx.push(allRtoI)
        }

        for( var fp=0 ; fp < foundPeers.length ; ++fp) {
            var fpIdent = foundPeers[fp].ident
            for( var rtoNewI=0 ; rtoNewI < hashIdentToNewState[fpIdent].realtimeObjects.length ; ++rtoNewI) {
                var rtoNew = hashIdentToNewState[fpIdent].realtimeObjects[rtoNewI]
                var foundRto = false
                var iRto = missingRtos.length
                while (iRto--) {
                    if(rtoNew.ident === missingRtos[iRto]) {
                        foundRtos.push(rtoNew)
                        foundRtosByIdent[rtoNew.ident] = rtoNew
                        missingRtos.splice(iRto, 1)
                        missingRtosModelIdx.splice(iRto, 1)
                        foundRto = true
                        break
                    }
                }
                if( !foundRto ) {
                    newRtos.push(rtoNew)
                }
            }
        }

        for( var np=0 ; np < newPeers.length ; ++np) {
            var npIdent = newPeers[np].ident
            for( var rtoNewJ=0 ; rtoNewJ < hashIdentToNewState[npIdent].realtimeObjects.length ; ++rtoNewJ) {
                var rtoNew2 = hashIdentToNewState[npIdent].realtimeObjects[rtoNewJ]
                newRtos.push(rtoNew2)
            }
        }

        //console.log("DBG: rto fin, new: " + newRtos.length + ", found: " + foundRtos.length + ", miss: " + missingRtos.length)

        // Update Visible EntityInfos (use foundPeers for that)
        var newVisObjs =[] // CREATE
        var foundVisObjs = [] // UPDATE
        var foundVisObjsByIdent = {} // UPDATE
        var missingVisObjs = [] // DELETE
        var missingVisObjsModelIdx = [] // DELETE
        for( var allVisObjsI=0 ; allVisObjsI < visibleEntityInfosList.count/**/ ; ++allVisObjsI) {
            var theVisObj = visibleEntityInfos[allVisObjsI]
            missingVisObjs.push(theVisObj.path)
            missingVisObjsModelIdx.push(allVisObjsI)
        }

        for( var fp=0 ; fp < foundPeers.length ; ++fp) {
            var fpIdent = foundPeers[fp].ident
            for( var visObjNewI=0 ; visObjNewI < hashIdentToNewState[fpIdent].visibleEntityInfos.length ; ++visObjNewI) {
                var visObjNew = hashIdentToNewState[fpIdent].visibleEntityInfos[visObjNewI]
                var foundVisObj = false
                var iVisObj = missingVisObjs.length
                while (iVisObj--) {
                    if(visObjNew.path === missingVisObjs[iVisObj]) {
                        foundVisObjs.push(visObjNew)
                        foundVisObjsByIdent[visObjNew.path] = visObjNew
                        missingVisObjs.splice(iVisObj, 1)
                        missingVisObjsModelIdx.splice(iVisObj, 1)
                        foundVisObj = true
                        break
                    }
                }
                if( !foundVisObj ) {
                    newVisObjs.push(visObjNew)
                }
            }
        }

        for( var np=0 ; np < newPeers.length ; ++np) {
            var npIdent = newPeers[np].ident
            for( var visObjNewJ=0 ; visObjNewJ < hashIdentToNewState[npIdent].visibleEntityInfos.length ; ++visObjNewJ) {
                var visObjNew2 = hashIdentToNewState[npIdent].visibleEntityInfos[visObjNewJ]
                newVisObjs.push(visObjNew2)
            }
        }

        worldUpdatedDiff( newPeers,   foundPeers,   missingPeers
                         ,newRtos,    foundRtos,    missingRtos
                         ,newVisObjs, foundVisObjs, missingVisObjs)

        // update peers
        for( var prsmiss=0 ; prsmiss < missingPeers.length ; ++prsmiss) {
            delete peerToPeerState[missingPeers[prsmiss]]
        }
        for( var prsupd=0 ; prsupd < foundPeers.length ; ++prsupd) {
            //VO BO mapping, Note: will not work for some types (vector, matrix, ...)
//            for( var prop in foundPeers[prsupd]) {
//                peerToPeerState[foundPeers[prsupd].ident][prop] = foundPeers[prsupd][prop]
//            }
            var peerIdentIndex = foundPeers[prsupd].ident

            //peerToPeerState[peerIdentIndex].sessionId = foundPeers[prsupd].sessionId
            console.log("DBG: JSON.stringify(world): " + JSON.stringify(world))
            console.log("DBG: foundPeers[x]: " + JSON.stringify(foundPeers[prsupd]))
            peerToPeerState[peerIdentIndex].ident = foundPeers[prsupd].ident
            peerToPeerState[peerIdentIndex].peername = foundPeers[prsupd].peername
            peerToPeerState[peerIdentIndex].isHost = foundPeers[prsupd].isHost
            peerToPeerState[peerIdentIndex].repositoryPort = foundPeers[prsupd].repositoryPort
            peerToPeerState[peerIdentIndex].checkoutName = foundPeers[prsupd].checkoutName
            //peerToPeerState[peerIdentIndex].realtimeObjects = foundPeers[prsupd].realtimeObjects //TODO: Add fromJson/toJSON Method to peerState/RealtimeObject/EntityVisualInfo
            //peerToPeerState[peerIdentIndex].visibleEntityInfos = foundPeers[prsupd].visibleEntityInfos
            peerToPeerState[peerIdentIndex].timestamp = foundPeers[prsupd].timestamp

        }
        console.log("DBG: newPeers: ")
        for( var prsnew=0 ; prsnew < newPeers.length ; ++prsnew) {
            var newPeerObj = newPeers[prsnew]
            console.log("  DBG: newPeer: " + newPeerObj.peername)
            //VO BO mapping
            var blueprint = {
                ident: newPeerObj.ident,
                peername: newPeerObj.peername,
//                visibleEntityInfos: newPeerObj.visibleEntityInfos,
//                realtimeObjects: newPeerObj.realtimeObjects,
                timestamp: newPeerObj.timestamp
            }
            peerToPeerState[newPeers[prsnew].ident] = peerStateComponent.createObject(null, blueprint)
        }

        console.log("DBG: for(missingRtosModelIdx): ")
        // update rto model
        // remove
        for( var mrmi=0 ; mrmi < missingRtosModelIdx.length ; ++mrmi) {
            var idx = missingRtosModelIdx[mrmi]
            console.log("  DBG: missingRtosModelIdx: " + idx)
            realtimeObjects.remove(idx, 1)
        }
        console.log("DBG: update realtimeObjects for(...): ")
        // update
        for( var rtoUpd=0 ; rtoUpd < realtimeObjects.count ; ++rtoUpd) {
            var theRto2 = realtimeObjects.get(rtoUpd)
            var newRtoState = foundRtosByIdent[theRto2.ident]
            if(!newRtoState) continue

            theRto2.tf = Qt.matrix4x4( newRtoState.tf )
            theRto2.vel = Qt.vector3d( newRtoState.vel[0], newRtoState.vel[1], newRtoState.vel[2] )
            theRto2.type = newRtoState.type
            theRto2.additionalData = newRtoState.additionalData
        }
        console.log("DBG: create realtimeObjects for(...): ")
        // create
        for( var rtoC=0 ; rtoC < newRtos.length ; ++rtoC) {
            var newRto = newRtos[rtoC]
            var blueprintRto = {
                ident: newRto.ident,
                peerOwner: newRto.peerOwner,
                tf: Qt.matrix4x4( newRto.tf ),
                vel: Qt.vector3d( newRto.vel[0], newRto.vel[1], newRto.vel[2] ),
                type: newRto.type,
                additionalData: newRto.additionalData
            }
            realtimeObjects.append(realtimeObjectComponent.createObject(null, blueprintRto))
        }
        console.log("DBG: Working on VisualObjectInfos: ")
        // update visibleEntityInfos model
        // remove
        for( var mrmi2=0 ; mrmi2 < missingVisObjsModelIdx.length ; ++mrmi2) {
            var idx2 = missingVisObjsModelIdx[mrmi2]
            //TODO: Array fails?
            var arr = visibleEntityInfos
            arr.splice(idx2, 1)
            visibleEntityInfos = arr
            visibleEntityInfosList.remove(idx2)
        }
        console.log("DBG: UPDATE VisualObjectInfos: ")
        // update
        for( var visObjUpd=0 ; visObjUpd < visibleEntityInfosList.count/**/ ; ++visObjUpd) {
            var theVisObj2 = visibleEntityInfos[visObjUpd]
            var newVisObjState = foundVisObjsByIdent[theVisObj2.path]
            if(!newVisObjState) continue

            theVisObj2.peerOwner = newVisObjState.peerOwner
            theVisObj2.additionalData = newRtoState.additionalData
        }
        console.log("DBG: CREATE VisualObjectInfos:for(...) ")
        // create
        for( var visObjC=0 ; visObjC < newVisObjs.length ; ++visObjC) {
            var newVisObj = newVisObjs[visObjC]
            var blueprintVisObj = {
                path: newVisObj.path,
                peerOwner: newVisObj.peerOwner,
                additionalData: newVisObj.additionalData
            }
            console.log("  DBG: ADDING NETWORK newVisObj: " + JSON.stringify(newVisObj))
            var arr = visibleEntityInfos
            arr.push(visibleObjectComponent.createObject(null, blueprintVisObj))
            visibleEntityInfos = arr
            visibleEntityInfosList.append({path: newVisObj.path})
        }
        console.log("DBG: Working on VisualObjectInfos DONE ")
    }
    Component {
        id: realtimeObjectComponent
        RealtimeObject {}
    }
    Component {
        id: visibleObjectComponent
        EntityVisualInfo {}
    }
    Component {
        id: peerStateComponent
        MapitMultiviewPeerState {}
    }

    function worldUpdatedDiff(peersCreate,  peersUpdate,  peersDelete,
                              rtoCreate,    rtoUpdate,    rtoDelete,
                              visObjCreate, visObjUpdate, visObjDelete) {

        for( var pc=0 ; pc < peersCreate.length ; ++pc) peerJoined(peersCreate[pc])
        for( var pu=0 ; pu < peersUpdate.length ; ++pu) peerUpdated(peersUpdate[pu])
        for( var pd=0 ; pd < peersDelete.length ; ++pd) peerLeft(peersDelete[pd])

        for( var rtoc=0 ; rtoc < rtoCreate.length ; ++rtoc) realtimeObjectAdded(rtoCreate[rtoc])
        for( var rtou=0 ; rtou < rtoUpdate.length ; ++rtou) realtimeObjectUpdated(rtoUpdate[rtou])
        for( var rtod=0 ; rtod < rtoDelete.length ; ++rtod) realtimeObjectRemoved(rtoDelete[rtod])

        for( var voc=0 ; voc < visObjCreate.length ; ++voc) visibleObjectAdded(visObjCreate[voc])
        for( var vou=0 ; vou < visObjUpdate.length ; ++vou) visibleObjectUpdated(visObjUpdate[vou])
        for( var vod=0 ; vod < visObjDelete.length ; ++vod) visibleObjectRemoved(visObjDelete[vod])
    }
}
