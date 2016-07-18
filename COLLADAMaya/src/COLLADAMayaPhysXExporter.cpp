/*
    Copyright (c) 2015 Starbreeze

	This file is part of COLLADAMaya.

    Portions of the code are:
    Copyright (c) 2005-2007 Feeling Software Inc.
    Copyright (c) 2005-2007 Sony Computer Entertainment America
    Copyright (c) 2004-2005 Alias Systems Corp.
	
    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#include "COLLADAMayaStableHeaders.h"

#include "COLLADAMayaAttributeParser.h"
#include "COLLADAMayaDagHelper.h"
#include "COLLADAMayaExportOptions.h"
#include "COLLADAMayaGeometryExporter.h"
#include "COLLADAMayaPhysXExporter.h"
#include "COLLADAMayaRotateHelper.h"
#include "COLLADAMayaSceneGraph.h"
#include "COLLADAMayaSyntax.h"
#include "COLLADAMayaVisualSceneExporter.h"
#include "COLLADASWConstants.h"
#include "Math/COLLADABUMathUtils.h"

#include <libxml/tree.h>
#include <maya/MAngle.h>
#include <maya/MDistance.h>
#include <maya/MFnAttribute.h>
#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#include <maya/MFnTransform.h>
#include <maya/MString.h>
#include <maya/MTime.h>

double infinite()
{
    union ieee754 {
        int64_t i;
        double d;
    };
    ieee754 inf;
    inf.i = 0x7FF0000000000000;
    return inf.d;
}

bool isInf(double d)
{
    return
        d == infinite() ||
        d == -infinite();
}

using namespace COLLADASW;

// Note:
// Maya matrix: row major vectors, T1 x T2 x T3...
// DAE matrix/transforms: column major vectors, ...T3 x T2 x T1
// -> reverse order

namespace COLLADAMaya
{
    // Helper class used to open and auto close an element.
    class Element
    {
    public:
        Element(PhysXExporter& exporter, const String & name)
            : mPhysXExporter(exporter)
        {
            mPhysXExporter.getStreamWriter().openElement(name);
        }

        virtual ~Element()
        {
            mPhysXExporter.getStreamWriter().closeElement();
        }

    protected:
        StreamWriter& getStreamWriter()
        {
            return mPhysXExporter.getStreamWriter();
        }

        DocumentExporter& getDocumentExporter()
        {
            return mPhysXExporter.getDocumentExporter();
        }

        PhysXExporter& getPhysXExporter()
        {
            return mPhysXExporter;
        }

    private:
        PhysXExporter& mPhysXExporter;
    };
    
    class AttributeExporter : public AttributeParser
    {
    public:
        AttributeExporter(PhysXExporter& exporter, const std::set<MString, MStringComp> & attributes)
            : mPhysXExporter(exporter)
            , mAttributes(attributes)
        {}

    protected:
        virtual bool onBeforeAttribute(MFnDependencyNode & node, MObject & attr) override
        {
            MStatus status;
            MFnAttribute fnAttr(attr, &status);
            if (!status) return false;

            MString attrName = fnAttr.name(&status);
            if (!status) return false;

            std::set<MString, MStringComp>::const_iterator it = mAttributes.find(attrName);
            if (it == mAttributes.end()) {
                return false;
            }

            return true;
        }

        virtual void onBoolean(MPlug & plug, const MString & name, bool value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value);
        }

        virtual void onByte(MPlug & plug, const MString & name, char value) override
        {
            String nameStr(name.asChar());
			const size_t size = 5;
			char text[size];
			snprintf(text, size, "0x%X", value);
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(text);
        }

        virtual void onChar(MPlug & plug, const MString & name, char value) override
        {
            String nameStr(name.asChar());
            char text[] = { value, '\0' };
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(text);
        }

        virtual void onShort(MPlug & plug, const MString & name, short value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value);
        }

        virtual void onShort2(MPlug & plug, const MString & name, short value[2]) override
        {
            String nameStr(name.asChar());
            int values[3] = { value[0], value[1] };
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(values, 2);
        }

        virtual void onShort3(MPlug & plug, const MString & name, short value[3]) override
        {

            String nameStr(name.asChar());
            int values[3] = { value[0], value[1], value[2] };
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(values, 3);
        }

        virtual void onInteger(MPlug & plug, const MString & name, int value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value);
        }

        virtual void onInteger2(MPlug & plug, const MString & name, int value[2]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 2);
        }

        virtual void onInteger3(MPlug & plug, const MString & name, int value[3]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 3);
        }

        virtual void onFloat(MPlug & plug, const MString & name, float value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value);
        }

        virtual void onFloat2(MPlug & plug, const MString & name, float value[2]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 2);
        }

        virtual void onFloat3(MPlug & plug, const MString & name, float value[3]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 3);
        }

        virtual void onDouble(MPlug & plug, const MString & name, double value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value);
        }

        virtual void onDouble2(MPlug & plug, const MString & name, double value[2]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 2);
        }

        virtual void onDouble3(MPlug & plug, const MString & name, double value[3]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 3);
        }

        virtual void onDouble4(MPlug & plug, const MString & name, double value[4]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(value, 4);
        }

        virtual void onString(MPlug & plug, const MString & name, const MString & value) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendText(value.asChar());
        }

        virtual void onEnum(MPlug & plug, const MString & name, int enumValue, const MString & enumName) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendText(enumName.asChar());
        }

        virtual void onMesh(MPlug & plug, const MString & name, MObject & meshObject) override
        {
            String nameStr(name.asChar());
            URI meshURI;
            mPhysXExporter.getMeshURI(meshObject, meshURI);
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_URL, meshURI);
        }

        virtual void onAngle(MPlug & plug, const MString & name, const MAngle & angle) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                angle.asDegrees());
        }

        virtual void onAngle2(MPlug & plug, const MString & name, const MAngle angle[2]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                angle[0].asDegrees(),
                angle[1].asDegrees());
        }

        virtual void onAngle3(MPlug & plug, const MString & name, const MAngle angle[3]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                angle[0].asDegrees(),
                angle[1].asDegrees(),
                angle[2].asDegrees());
        }

        virtual void onAngle4(MPlug & plug, const MString & name, const MAngle angle[4]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                angle[0].asDegrees(),
                angle[1].asDegrees(),
                angle[2].asDegrees(),
                angle[3].asDegrees());
        }

        virtual void onDistance(MPlug & plug, const MString & name, const MDistance & distance) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                MDistance::internalToUI(distance.asCentimeters()));
        }

        virtual void onDistance2(MPlug & plug, const MString & name, const MDistance distance[2]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                MDistance::internalToUI(distance[0].asCentimeters()),
                MDistance::internalToUI(distance[1].asCentimeters()));
        }

        virtual void onDistance3(MPlug & plug, const MString & name, const MDistance distance[3]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                MDistance::internalToUI(distance[0].asCentimeters()),
                MDistance::internalToUI(distance[1].asCentimeters()),
                MDistance::internalToUI(distance[2].asCentimeters()));
        }

        virtual void onDistance4(MPlug & plug, const MString & name, const MDistance distance[4]) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(
                MDistance::internalToUI(distance[0].asCentimeters()),
                MDistance::internalToUI(distance[1].asCentimeters()),
                MDistance::internalToUI(distance[2].asCentimeters()),
                MDistance::internalToUI(distance[3].asCentimeters()));
        }

        virtual void onTime(MPlug & plug, const MString & name, MTime & time) override
        {
            String nameStr(name.asChar());
            Element e(mPhysXExporter, nameStr);
            mPhysXExporter.getStreamWriter().appendValues(time.as(MTime::kSeconds));
        }

    private:
        PhysXExporter& mPhysXExporter;
        const std::set<MString, MStringComp>& mAttributes;
        int mAttributeLevel;
    };

    void PhysXRigidConstraint::GetSwingConeAndTwistMinLimit(const MObject & rigidConstraint, MVector & min)
    {
        int dummy = 0;
        MString motionSwingY;
        MString motionSwingZ;
        MString motionTwist;
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_SWING_Y, dummy, motionSwingY);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_SWING_Z, dummy, motionSwingZ);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_TWIST, dummy, motionTwist);

        if (motionSwingY == LOCKED) {
            min.x = 0.0;
        }
        else if (motionSwingY == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_SWING_1_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            min.x = -limit;
        }
        else if (motionSwingY == FREE) {
            min.x = -infinite();
        }

        if (motionSwingZ == LOCKED) {
            min.y = 0.0;
        }
        else if (motionSwingZ == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_SWING_2_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            min.y = -limit;
        }
        else if (motionSwingZ == FREE) {
            min.y = -infinite();
        }

        if (motionTwist == LOCKED) {
            min.z = 0.0;
        }
        else if (motionTwist == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_TWIST_LOW_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            min.z = limit;
        }
        else if (motionTwist == FREE) {
            min.z = -infinite();
        }
    }

    void PhysXRigidConstraint::GetSwingConeAndTwistMaxLimit(const MObject & rigidConstraint, MVector & max)
    {
        int dummy = 0;
        MString motionSwingY;
        MString motionSwingZ;
        MString motionTwist;
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_SWING_Y, dummy, motionSwingY);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_SWING_Z, dummy, motionSwingZ);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_TWIST, dummy, motionTwist);

        if (motionSwingY == LOCKED) {
            max.x = 0.0;
        }
        else if (motionSwingY == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_SWING_1_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            max.x = limit;
        }
        else if (motionSwingY == FREE) {
            max.x = infinite();
        }

        if (motionSwingZ == LOCKED) {
            max.y = 0.0;
        }
        else if (motionSwingZ == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_SWING_2_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            max.y = limit;
        }
        else if (motionSwingZ == FREE) {
            max.y = infinite();
        }

        if (motionTwist == LOCKED) {
            max.z = 0.0;
        }
        else if (motionTwist == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_TWIST_HIGH_LIMIT_VALUE, limit);
            limit = COLLADABU::Math::Utils::radToDeg(limit);
            max.z = limit;
        }
        else if (motionTwist == FREE) {
            max.z = infinite();
        }
    }
    
    void PhysXRigidConstraint::GetLinearMinLimit(const MObject & rigidConstraint, MVector & min)
    {
        int dummy = 0;
        MString motionX;
        MString motionY;
        MString motionZ;
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_X, dummy, motionX);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_Y, dummy, motionY);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_Z, dummy, motionZ);

        if (motionX == LOCKED) {
            min.x = 0.0;
        }
        else if (motionX == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            min.x = -limit;
        }
        else if (motionX == FREE) {
            min.x = -infinite();
        }

        if (motionY == LOCKED) {
            min.y = 0.0;
        }
        else if (motionY == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            min.y = -limit;
        }
        else if (motionY == FREE) {
            min.y = -infinite();
        }

        if (motionZ == LOCKED) {
            min.z = 0.0;
        }
        else if (motionZ == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            min.z = -limit;
        }
        else if (motionZ == FREE) {
            min.z = -infinite();
        }
    }

    void PhysXRigidConstraint::GetLinearMaxLimit(const MObject & rigidConstraint, MVector & max)
    {
        int dummy = 0;
        MString motionX;
        MString motionY;
        MString motionZ;
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_X, dummy, motionX);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_Y, dummy, motionY);
        DagHelper::getPlugValue(rigidConstraint, ATTR_MOTION_Z, dummy, motionZ);

        if (motionX == LOCKED) {
            max.x = 0.0;
        }
        else if (motionX == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            max.x = limit;
        }
        else if (motionX == FREE) {
            max.x = infinite();
        }

        if (motionY == LOCKED) {
            max.y = 0.0;
        }
        else if (motionY == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            max.y = limit;
        }
        else if (motionY == FREE) {
            max.y = infinite();
        }

        if (motionZ == LOCKED) {
            max.z = 0.0;
        }
        else if (motionZ == LIMITED) {
            double limit = 0.0;
            DagHelper::getPlugValue(rigidConstraint, ATTR_LINEAR_LIMIT_VALUE, limit);
            limit = MDistance::internalToUI(limit);
            max.z = limit;
        }
        else if (motionZ == FREE) {
            max.z = infinite();
        }
    }

    void PhysXRigidConstraint::GetSpringAngularStiffness(const MObject & rigidConstraint, double & stiffness)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_DRIVE_SPRING_SWING, stiffness);
        stiffness = COLLADABU::Math::Utils::radToDeg(stiffness);
    }

    void PhysXRigidConstraint::GetSpringAngularDamping(const MObject & rigidConstraint, double & damping)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_DRIVE_DAMPING_SWING, damping);
        damping = COLLADABU::Math::Utils::radToDeg(damping);
    }

    void PhysXRigidConstraint::GetSpringAngularTargetValue(const MObject & rigidConstraint, double & targetValue)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_GOAL_ORIENTATION_X, targetValue);
        targetValue = COLLADABU::Math::Utils::radToDeg(targetValue);
    }

    void PhysXRigidConstraint::GetSpringLinearStiffness(const MObject & rigidConstraint, double & stiffness)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_DRIVE_SPRING_X, stiffness);
    }

    void PhysXRigidConstraint::GetSpringLinearDamping(const MObject & rigidConstraint, double & damping)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_DRIVE_DAMPING_X, damping);
    }

    void PhysXRigidConstraint::GetSpringLinearTargetValue(const MObject & rigidConstraint, double & targetValue)
    {
        DagHelper::getPlugValue(rigidConstraint, ATTR_GOAL_POSITION_X, targetValue);
        targetValue = MDistance::internalToUI(targetValue);
    }

    void PhysXShape::GetType(const MObject & shape, MString & type)
    {
        int dummy = 0;
        DagHelper::getPlugValue(shape, ATTR_SHAPE_TYPE, dummy, type);
    }

    void PhysXShape::GetInMesh(const MObject & shape, MObject & mesh)
    {
        DagHelper::getPlugValue(shape, ATTR_IN_MESH, mesh);
    }

    void PhysXShape::GetConnectedInMesh(const MObject & shape, MObject & mesh)
    {
        PhysXExporter::GetPluggedObject(shape, ATTR_IN_MESH, mesh);
    }

    PhysXXML::PxMaterial* PhysXExporter::findPxMaterial(const MObject& rigidBody)
    {
        int dummy = 0;
        MString simulationType;
        DagHelper::getPlugValue(rigidBody, ATTR_SIMULATION_TYPE, dummy, simulationType);

        PhysXXML::GlobalPose* pGlobalPose = NULL;

        if (simulationType == SIMULATION_TYPE_STATIC) {
            PhysXXML::PxRigidStatic* pxRigidStatic = findPxRigidStatic(rigidBody);
            if (pxRigidStatic && pxRigidStatic->shapes.shapes.size() > 0) {
                // All shapes in a rigid body have the same material
                return findPxMaterial(pxRigidStatic->shapes.shapes[0].materials.materialRef.materialRef);
            }
        }
        else {
            PhysXXML::PxRigidDynamic* pxRigidDynamic = findPxRigidDynamic(rigidBody);
            if (pxRigidDynamic && pxRigidDynamic->shapes.shapes.size() > 0) {
                // All shapes in a rigid body have the same material
                return findPxMaterial(pxRigidDynamic->shapes.shapes[0].materials.materialRef.materialRef);
            }
        }
        return NULL;
    }

    PhysXXML::PxMaterial* PhysXExporter::findPxMaterial(uint64_t ref)
    {
        return mPhysXDoc->findMaterial(ref);
    }

    PhysXXML::PxShape* PhysXExporter::findPxShape(const MObject& rigidBody, const MObject& shape)
    {
        // Shape node
        MFnDagNode shapeNode(shape);
        MString shapeName = shapeNode.fullPathName();

        // Rigid body target
        MObject target;
        getRigidBodyTarget(rigidBody, target);
        MFnDagNode targetNode(target);
        MString targetName = targetNode.fullPathName();

        return mPhysXDoc->findShape(targetName.asChar(), shapeName.asChar());
    }

	PhysXXML::PxRigidStatic* PhysXExporter::findPxRigidStatic(uint64_t id)
	{
		return mPhysXDoc->findRigidStatic(id);
	}

    PhysXXML::PxRigidStatic* PhysXExporter::findPxRigidStatic(const MObject& rigidBody)
    {
        MObject target;
        getRigidBodyTarget(rigidBody, target);
        MFnDagNode targetNode(target);
        MString targetName = targetNode.fullPathName();
        return findPxRigidStatic(targetName.asChar());
    }

    PhysXXML::PxRigidStatic* PhysXExporter::findPxRigidStatic(const String& name)
    {
        return mPhysXDoc->findRigidStatic(name);
    }

	PhysXXML::PxRigidDynamic* PhysXExporter::findPxRigidDynamic(uint64_t id)
	{
		return mPhysXDoc->findRigidDynamic(id);
	}

    PhysXXML::PxRigidDynamic* PhysXExporter::findPxRigidDynamic(const MObject& rigidBody)
    {
        MObject target;
        getRigidBodyTarget(rigidBody, target);
        MFnDagNode targetNode(target);
        MString targetName = targetNode.fullPathName();
        return mPhysXDoc->findRigidDynamic(targetName.asChar());
    }

    PhysXXML::PxD6Joint* PhysXExporter::findPxD6Joint(const MObject& rigidConstraint)
    {
        MFnDagNode constraintNode(rigidConstraint);
        MString constraintName = constraintNode.fullPathName();
        return mPhysXDoc->findD6Joint(constraintName.asChar());
    }

	MObject PhysXExporter::getNodeRigidBody(const MObject& node)
	{
		if (node.isNull())
			return MObject::kNullObj;

		class RigidBodyParser
		{
		public:
			RigidBodyParser(PhysXExporter & exporter, const MObject & node)
				: mPhysXExporter(exporter)
				, mNode(node)
			{}

			bool operator()(SceneElement & element)
			{
				if (element.getType() == SceneElement::PHYSX_RIGID_BODY &&
					element.getIsLocal())
				{
					const MObject & rigidBody = element.getNode();

					MObject target;
					mPhysXExporter.getRigidBodyTarget(rigidBody, target);

					if (target == mNode)
					{
						mRigidBody = rigidBody;
						return false;
					}
				}
				return true;
			}

			const MObject & getRigidBody() const
			{
				return mRigidBody;
			}

		private:
			PhysXExporter & mPhysXExporter;
			const MObject & mNode;
			MObject mRigidBody;
		};

		RigidBodyParser parser(*this, node);
		parseSceneElements(parser);
		return parser.getRigidBody();
	}

    MObject PhysXExporter::getShapeRigidBody(const MObject& shape)
    {
        class FindShapeRigidBody
        {
        public:
            FindShapeRigidBody(PhysXExporter & exporter, const MObject& shape)
                : mPhysXExporter(exporter)
                , mShape(shape)
            {}

            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_BODY &&
                    element.getIsLocal())
                {
                    std::vector<MObject> shapes;
                    PhysXExporter::GetRigidBodyShapes(element.getNode(), shapes);

                    for (std::vector<MObject>::const_iterator it = shapes.begin(); it != shapes.end(); ++it)
                    {
                        if (*it == mShape)
                        {
                            mRigidBody = element.getNode();
                            // Stop parsing
                            return false;
                        }
                    }
                }
                return true;
            }

            MObject getRigidBody() const
            {
                return mRigidBody;
            }

        private:
            PhysXExporter & mPhysXExporter;
            MObject mShape;
            MObject mRigidBody;
        };

        FindShapeRigidBody parser(*this, shape);
        parseSceneElements(parser);
        return parser.getRigidBody();
    }

    void PhysXExporter::getShapeLocalPose(const MObject& rigidBody, const MObject& shape, MMatrix& localPose)
    {
        PhysXXML::PxShape* pxShape = findPxShape(rigidBody, shape);
        if (!pxShape)
            return;

        MTransformationMatrix tm;

        tm.setRotationQuaternion(
            pxShape->localPose.rotation.x,
            pxShape->localPose.rotation.y,
            pxShape->localPose.rotation.z,
            pxShape->localPose.rotation.w);
        
        MVector translation(
            MDistance::uiToInternal(pxShape->localPose.translation.x),
            MDistance::uiToInternal(pxShape->localPose.translation.y),
            MDistance::uiToInternal(pxShape->localPose.translation.z)
            );

        tm.setTranslation(translation, MSpace::kTransform);

        localPose = tm.asMatrix();
    }

    bool PhysXExporter::getShapeVertices(const MObject& shape, std::vector<PhysXXML::Point> & vertices, MString & meshId)
    {
        MObject rigidBody = getShapeRigidBody(shape);
        if (rigidBody.isNull()) return false;

        PhysXXML::PxShape* pxShape = findPxShape(rigidBody, shape);
        if (pxShape == NULL) return false;

        if (pxShape->geometry.type == PhysXXML::Geometry::ConvexMesh)
        {
            if (PhysXXML::PxConvexMesh* convexMesh = mPhysXDoc->findConvexMesh(pxShape->geometry.convexMeshGeometry.convexMesh.convexMesh))
            {
                vertices = convexMesh->points.points;
                std::stringstream s;
                s << convexMesh->id.id;
                meshId = s.str().c_str();
                return true;
            }
        }
        else if (pxShape->geometry.type == PhysXXML::Geometry::TriangleMesh)
        {
            if (PhysXXML::PxTriangleMesh* triangleMesh = mPhysXDoc->findTriangleMesh(pxShape->geometry.triangleMeshGeometry.triangleMesh.triangleMesh))
            {
                vertices = triangleMesh->points.points;
                std::stringstream s;
                s << triangleMesh->id.id;
                meshId = s.str().c_str();
                return true;
            }
        }
        return false;
    }

    bool PhysXExporter::getShapeTriangles(const MObject& shape, std::vector<PhysXXML::Triangle> & triangles)
    {
        MObject rigidBody = getShapeRigidBody(shape);
        if (rigidBody.isNull()) return false;

        PhysXXML::PxShape* pxShape = findPxShape(rigidBody, shape);
        if (pxShape == NULL) return false;

        if (pxShape->geometry.type == PhysXXML::Geometry::TriangleMesh)
        {
            if (PhysXXML::PxTriangleMesh* triangleMesh = mPhysXDoc->findTriangleMesh(pxShape->geometry.triangleMeshGeometry.triangleMesh.triangleMesh))
            {
                triangles = triangleMesh->triangles.triangles;
                return true;
            }
        }
        return false;
    }

    void PhysXExporter::getRigidBodyGlobalPose(const MObject& rigidBody, MMatrix& globalPose)
    {
        int dummy = 0;
        MString simulationType;
        DagHelper::getPlugValue(rigidBody, ATTR_SIMULATION_TYPE, dummy, simulationType);

        PhysXXML::GlobalPose* pGlobalPose = NULL;

        if (simulationType == SIMULATION_TYPE_STATIC) {
            PhysXXML::PxRigidStatic* pxRigidStatic = findPxRigidStatic(rigidBody);
            if (pxRigidStatic) {
                pGlobalPose = &pxRigidStatic->globalPose;
            }
        }
        else {
            PhysXXML::PxRigidDynamic* pxRigidDynamic = findPxRigidDynamic(rigidBody);
            if (pxRigidDynamic) {
                pGlobalPose = &pxRigidDynamic->globalPose;
            }
        }

        if (!pGlobalPose)
            return;

        MTransformationMatrix tm;

        tm.setRotationQuaternion(
            pGlobalPose->rotation.x,
            pGlobalPose->rotation.y,
            pGlobalPose->rotation.z,
            pGlobalPose->rotation.w);

        MVector translation(
            MDistance::uiToInternal(pGlobalPose->translation.x),
            MDistance::uiToInternal(pGlobalPose->translation.y),
            MDistance::uiToInternal(pGlobalPose->translation.z)
            );

        tm.setTranslation(translation, MSpace::kTransform);

        globalPose = tm.asMatrix();
    }

    void PhysXExporter::getRigidBodyTarget(const MObject& rigidBody, MObject& target)
    {
        target = MObject::kNullObj;

        PhysXExporter::GetPluggedObject(rigidBody, ATTR_TARGET, target);

        // If target is null then use rigidBody as target
        if (target.isNull())
        {
            target = rigidBody;
        }
    }
        
    bool PhysXExporter::getRigidSolver(MObject & rigidSolver)
    {
        class RigidSolverParser
        {
        public:            
            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_SOLVER &&
                    element.getIsLocal())
                {
                    mRigidSolver = element.getNode();
                    return false;
                }
                return true;
            }
            
            const MObject & getRigidSolver()
            {
                return mRigidSolver;
            }
            
        private:
            MObject mRigidSolver;
        } parser;
        parseSceneElements(parser);
        rigidSolver = parser.getRigidSolver();
        return !rigidSolver.isNull();
    }

    class Dynamic : public Element
    {
    public:
        Dynamic(PhysXExporter& exporter, bool value)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_DYNAMIC)
        {
            getStreamWriter().appendValues(value);
        }
    };

    class Mass : public Element
    {
    public:
        Mass(PhysXExporter& exporter, double mass)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_MASS)
        {
            getStreamWriter().appendValues(mass);
        }
    };

    class MassFrame : public Element
    {
    public:
        MassFrame(PhysXExporter& exporter, const MVector & translation, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_MASS_FRAME)
        {
            getPhysXExporter().exportTranslation(translation, sid);
        }
    };

    class Inertia : public Element
    {
    public:
        Inertia(PhysXExporter& exporter, const MVector & inertia)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_INERTIA)
        {
            getStreamWriter().appendValues(inertia.x, inertia.y, inertia.z);
        }
    };

    class Restitution : public Element
    {
    public:
        Restitution(PhysXExporter& exporter, double restitution)
            : Element(exporter, CSWC::CSW_ELEMENT_RESTITUTION)
        {
            getStreamWriter().appendValues(restitution);
        }

		static double DefaultValue()
		{
			return 0.0;
		}
    };

    class DynamicFriction : public Element
    {
    public:
        DynamicFriction(PhysXExporter& exporter, double dynamicFriction)
            : Element(exporter, CSWC::CSW_ELEMENT_DYNAMIC_FRICTION)
        {
            getStreamWriter().appendValues(dynamicFriction);
        }
    };

    class StaticFriction : public Element
    {
    public:
        StaticFriction(PhysXExporter& exporter, double staticFriction)
            : Element(exporter, CSWC::CSW_ELEMENT_STATIC_FRICTION)
        {
            getStreamWriter().appendValues(staticFriction);
        }
    };

	class FrictionCombineMode : public Element
	{
	public:
		FrictionCombineMode(PhysXExporter & exporter, const PhysXXML::CombineMode::FlagEnum & combineMode)
			: Element(exporter, CSWC::CSW_ELEMENT_FRICTION_COMBINE_MODE)
		{
			getStreamWriter().appendValues(PhysXExporter::CombineModeToCOLLADA(combineMode));
		}

		static PhysXXML::CombineMode::FlagEnum DefaultValue()
		{
			return PhysXXML::CombineMode::FlagEnum::Average;
		}
	};

	class RestitutionCombineMode : public Element
	{
	public:
		RestitutionCombineMode(PhysXExporter & exporter, const PhysXXML::CombineMode::FlagEnum & combineMode)
			: Element(exporter, CSWC::CSW_ELEMENT_RESTITUTION_COMBINE_MODE)
		{
			getStreamWriter().appendValues(PhysXExporter::CombineModeToCOLLADA(combineMode));
		}

		static PhysXXML::CombineMode::FlagEnum DefaultValue()
		{
			return PhysXXML::CombineMode::FlagEnum::Average;
		}
	};

    class PhysicsMaterialTechnique : public Element
    {
    public:
        PhysicsMaterialTechnique(PhysXExporter& exporter, const MObject& rigidBody, const String& profile)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, profile);
			if (profile == PhysXExporter::GetPhysXProfile())
			{
				PhysXXML::PxMaterial* mat = getPhysXExporter().findPxMaterial(rigidBody);
				if (mat)
				{
					exportFrictionCombineMode(*mat);
					exportRestitutionCombineMode(*mat);
				}
			}
            if (profile == PhysXExporter::GetProfileXML()) {
                exporter.exportMaterialPhysXXML(rigidBody);
            }
        }

	private:
		void exportFrictionCombineMode(const PhysXXML::PxMaterial & mat)
		{
			if (mat.frictionCombineMode.frictionCombineMode != FrictionCombineMode::DefaultValue())
			{
				FrictionCombineMode e(getPhysXExporter(), mat.frictionCombineMode.frictionCombineMode);
			}
		}

		void exportRestitutionCombineMode(const PhysXXML::PxMaterial & mat)
		{
			if (mat.restitutionCombineMode.restitutionCombineMode != RestitutionCombineMode::DefaultValue())
			{
				RestitutionCombineMode e(getPhysXExporter(), mat.restitutionCombineMode.restitutionCombineMode);
			}
		}
    };

    class PhysicsMaterialExtra : public Element
    {
    public:
        PhysicsMaterialExtra(PhysXExporter& exporter, const MObject& rigidBody)
            : Element(exporter, CSWC::CSW_ELEMENT_EXTRA)
        {
			exportTechnique(rigidBody, PhysXExporter::GetPhysXProfile());
            deprecated_exportTechnique(rigidBody, PhysXExporter::GetProfileXML());
        }

    private:
		void exportTechnique(const MObject & rigidBody, const String & profile)
		{
			PhysicsMaterialTechnique e(getPhysXExporter(), rigidBody, profile);
		}

        void deprecated_exportTechnique(const MObject& rigidBody, const String& profile)
        {
            PhysicsMaterialTechnique e(getPhysXExporter(), rigidBody, profile);
        }
    };

    class PhysicsMaterialTechniqueCommon : public Element
    {
    public:
        PhysicsMaterialTechniqueCommon(PhysXExporter& exporter, const MObject& rigidBody)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE_COMMON)
        {
			exportDynamicFriction(rigidBody);
            exportRestitution(rigidBody);
            exportStaticFriction(rigidBody);
        }

    private:
        void exportRestitution(const MObject & rigidBody)
        {
            double restitution = 0.0;
            DagHelper::getPlugValue(rigidBody, ATTR_BOUNCINESS, restitution);
            if (restitution != 0.0)
            {
                Restitution e(getPhysXExporter(), restitution);
            }
        }

        void exportDynamicFriction(const MObject & rigidBody)
        {
            double dynamicFriction = 0.0;
            DagHelper::getPlugValue(rigidBody, ATTR_DYNAMIC_FRICTION, dynamicFriction);
            if (dynamicFriction != 0.0)
            {
                DynamicFriction e(getPhysXExporter(), dynamicFriction);
            }
        }

        void exportStaticFriction(const MObject & rigidBody)
        {
            double staticFriction = 0.0;
            DagHelper::getPlugValue(rigidBody, ATTR_STATIC_FRICTION, staticFriction);
            if (staticFriction != 0.0)
            {
                StaticFriction e(getPhysXExporter(), staticFriction);
            }
        }
    };

    class PhysicsMaterial : public Element
    {
    public:
        PhysicsMaterial(PhysXExporter& exporter, const MObject & rigidBody)
            : Element(exporter, CSWC::CSW_ELEMENT_PHYSICS_MATERIAL)
        {
            MDagPath rigidBodyDagPath;
            MDagPath::getAPathTo(rigidBody, rigidBodyDagPath);

            String rigidBodyId = getPhysXExporter().generateColladaId(rigidBodyDagPath);
            String materialId = rigidBodyId + "_material";

            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_ID, materialId);

            exportTechniqueCommon(rigidBody);
            exportExtra(rigidBody);
        }

    private:
        void exportTechniqueCommon(const MObject& rigidBody)
        {
            PhysicsMaterialTechniqueCommon e(getPhysXExporter(), rigidBody);
        }

        void exportExtra(const MObject& rigidBody)
        {
            PhysicsMaterialExtra e(getPhysXExporter(), rigidBody);
        }
    };

    class Hollow : public Element
    {
    public:
        Hollow(PhysXExporter& exporter, bool hollow)
            : Element(exporter, CSWC::CSW_ELEMENT_HOLLOW)
        {
            getStreamWriter().appendValues(hollow);
        }
    };

    class HalfExtents : public Element
    {
    public:
        HalfExtents(PhysXExporter& exporter, const MVector & halfExtents)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_BOX_EXTENTS)
        {
            getStreamWriter().appendValues(halfExtents.x, halfExtents.y, halfExtents.z);
        }
    };

    class ShapeBox : public Element
    {
    public:
		ShapeBox(PhysXExporter& exporter, const MObject & rigidBody, const MObject & shape)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_BOX)
        {
            exportHalfExtents(rigidBody, shape);
        }

    private:
		void exportHalfExtents(const MObject & rigidBody, const MObject & shape)
        {
			PhysXXML::PxShape* pxShape = getPhysXExporter().findPxShape(rigidBody, shape);
			if (pxShape)
			{
				MVector halfExtents = pxShape->geometry.boxGeometry.halfExtents.halfExtents;
				HalfExtents e(getPhysXExporter(), halfExtents);
			}
        }
    };

    class CapsuleRadius : public Element
    {
    public:
        CapsuleRadius(PhysXExporter& exporter, double radiusX, double radiusZ)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE_RADIUS)
        {
            getStreamWriter().appendValues(radiusX, radiusZ);
        }
    };

    class SphereRadius : public Element
    {
    public:
        SphereRadius(PhysXExporter& exporter, double radius)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE_RADIUS)
        {
            getStreamWriter().appendValues(radius);
        }
    };

    class ShapeSphere : public Element
    {
    public:
		ShapeSphere(PhysXExporter& exporter, const MObject & rigidBody, const MObject & shape)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_SPHERE)
        {
            exportRadius(rigidBody, shape);
        }

    private:
        void exportRadius(const MObject & rigidBody, const MObject & shape)
        {
			PhysXXML::PxShape* pxShape = getPhysXExporter().findPxShape(rigidBody, shape);
			if (pxShape)
			{
				SphereRadius e(getPhysXExporter(), pxShape->geometry.sphereGeometry.radius.radius);
			}
        }
    };

    class Height : public Element
    {
    public:
        Height(PhysXExporter& exporter, double height)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE_HEIGHT)
        {
            getStreamWriter().appendValues(height);
        }
    };

    class ShapeCapsule : public Element
    {
    public:
		ShapeCapsule(PhysXExporter& exporter, const MObject & rigidBody, const MObject & shape)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_CAPSULE)
        {
			PhysXXML::PxShape* pxShape = getPhysXExporter().findPxShape(rigidBody, shape);
			if (pxShape)
			{
				exportRadius(*pxShape);
				exportHeight(*pxShape);
			}
        }

    private:
        void exportRadius(const PhysXXML::PxShape & shape)
        {
            CapsuleRadius e(getPhysXExporter(), shape.geometry.capsuleGeometry.radius.radius, shape.geometry.capsuleGeometry.radius.radius);
        }

		void exportHeight(const PhysXXML::PxShape & shape)
        {
			Height e(getPhysXExporter(), shape.geometry.capsuleGeometry.halfHeight.halfHeight * 2.0);
        }
    };

    class InstanceGeometry : public Element
    {
    public:
        InstanceGeometry(PhysXExporter& exporter, const URI & geometryURI)
            : Element(exporter, CSWC::CSW_ELEMENT_INSTANCE_GEOMETRY)
        {
            getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_URL, geometryURI);
        }
    };

    class Density : public Element
    {
    public:
        Density(PhysXExporter& exporter, double density)
            : Element(exporter, CSWC::CSW_ELEMENT_DENSITY)
        {
            getStreamWriter().appendValues(density);
        }
    };

    class LocalPose : public Element
    {
    public:
        LocalPose(PhysXExporter& exporter, const MQuaternion& rotation, const MVector& translation)
            : Element(exporter, LOCAL_POSE)
        {
            double localPose[] = { rotation.x, rotation.y, rotation.z, rotation.w, translation.x, translation.y, translation.z };
            getStreamWriter().appendValues(localPose, sizeof(localPose) / sizeof(localPose[0]));
        }
    };

	class SimulationFilterData : public Element
	{
	public:
		SimulationFilterData(PhysXExporter & exporter, int f0, int f1, int f2, int f3)
			: Element(exporter, CSWC::CSW_ELEMENT_SIMULATION_FILTER_DATA)
		{
			getStreamWriter().appendValues(f0, f1, f2, f3);
		}
	};

	class QueryFilterData : public Element
	{
	public:
		QueryFilterData(PhysXExporter & exporter, int f0, int f1, int f2, int f3)
			: Element(exporter, CSWC::CSW_ELEMENT_QUERY_FILTER_DATA)
		{
			getStreamWriter().appendValues(f0, f1, f2, f3);
		}
	};

	class ContactOffset : public Element
	{
	public:
		ContactOffset(PhysXExporter & exporter, double contactOffset)
			: Element(exporter, CSWC::CSW_ELEMENT_CONTACT_OFFSET)
		{
			getStreamWriter().appendValues(contactOffset);
		}
	};

	class RestOffset : public Element
	{
	public:
		RestOffset(PhysXExporter & exporter, double restOffset)
			: Element(exporter, CSWC::CSW_ELEMENT_REST_OFFSET)
		{
			getStreamWriter().appendValues(restOffset);
		}
	};

	class ShapeFlags : public Element
	{
	public:
		ShapeFlags(PhysXExporter & exporter, const Flags<PhysXXML::ShapeFlags::FlagEnum> & flags)
			: Element(exporter, CSWC::CSW_ELEMENT_SHAPE_FLAGS)
		{
			getStreamWriter().appendText(PhysXExporter::ShapeFlagsToCOLLADA(flags));
		}

		static Flags<PhysXXML::ShapeFlags::FlagEnum> DefaultValue()
		{
			return Flags<PhysXXML::ShapeFlags::FlagEnum>(
				PhysXXML::ShapeFlags::FlagEnum::Visualization |
				PhysXXML::ShapeFlags::FlagEnum::SimulationShape |
				PhysXXML::ShapeFlags::FlagEnum::SceneQueryShape);
		}
	};

	class DebugName : public Element
	{
	public:
		DebugName(PhysXExporter & exporter, const String & name)
			: Element(exporter, CSWC::CSW_ELEMENT_NAME)
		{
			getStreamWriter().appendText(name);
		}
	};

    class ShapeTechnique : public Element
    {
    public:
        ShapeTechnique(PhysXExporter& exporter, const MObject& rigidBody, const MObject& shape, const String & profile)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, profile);
            //exportLocalPose(shape);
            if (profile == PhysXExporter::GetProfile()) {
                exporter.exportAttributes(shape, GetAttributes());
            }
            else if (profile == PhysXExporter::GetProfileXML()) {
                exporter.exportShapePhysXXML(rigidBody, shape);
            }
            else if (profile == PROFILE_MAYA) {
                exporter.exportExtraAttributes(shape);
            }
			else if (profile == PhysXExporter::GetPhysXProfile()) {
				PhysXXML::PxShape* pxShape = getPhysXExporter().findPxShape(rigidBody, shape);
				if (pxShape)
				{
					exportSimulationFilterData(*pxShape);
					exportQueryFilterData(*pxShape);
					exportContactOffset(*pxShape);
					exportRestOffset(*pxShape);
					exportShapeFlags(*pxShape);
					exportName(*pxShape);
				}
			}
        }

    private:
        static const std::set<MString, MStringComp>& GetAttributes()
        {
            if (mAttributes.size() == 0)
            {
                // Attributes we want to export in <extra> section
                mAttributes.insert(ATTR_SHAPE_TYPE);
                mAttributes.insert(ATTR_SIZE);
                mAttributes.insert(ATTR_RADIUS);
                mAttributes.insert(ATTR_HEIGHT);
                mAttributes.insert(ATTR_BEST_FIT);
                mAttributes.insert(ATTR_OUT_PHYSICS_SHAPE);
                mAttributes.insert(ATTR_CONNECT_TO_CLOTH_SPHERE);
                mAttributes.insert(ATTR_INFLATE);
                mAttributes.insert(ATTR_USE_MASS_OR_DENSITY);
                mAttributes.insert(ATTR_MASS);
                mAttributes.insert(ATTR_DENSITY);
            }
            return mAttributes;
        }

		void exportSimulationFilterData(const PhysXXML::PxShape & shape)
		{
			if (shape.simulationFilterData.filter0 != 0 ||
				shape.simulationFilterData.filter1 != 0 ||
				shape.simulationFilterData.filter2 != 0 ||
				shape.simulationFilterData.filter3 != 0)
			{
				SimulationFilterData e(
					getPhysXExporter(),
					shape.simulationFilterData.filter0,
					shape.simulationFilterData.filter1,
					shape.simulationFilterData.filter2,
					shape.simulationFilterData.filter3
					);
			}
		}

		void exportQueryFilterData(const PhysXXML::PxShape & shape)
		{
			if (shape.queryFilterData.filter0 != 0 ||
				shape.queryFilterData.filter1 != 0 ||
				shape.queryFilterData.filter2 != 0 ||
				shape.queryFilterData.filter3 != 0)
			{
				QueryFilterData e(
					getPhysXExporter(),
					shape.queryFilterData.filter0,
					shape.queryFilterData.filter1,
					shape.queryFilterData.filter2,
					shape.queryFilterData.filter3
					);
			}
		}

		void exportContactOffset(const PhysXXML::PxShape & shape)
		{
			if (shape.contactOffset.contactOffset != 0.02)
			{
				ContactOffset e(getPhysXExporter(), shape.contactOffset.contactOffset);
			}
		}

		void exportRestOffset(const PhysXXML::PxShape & shape)
		{
			if (shape.restOffset.restOffset != 0.0)
			{
				RestOffset e(getPhysXExporter(), shape.restOffset.restOffset);
			}
		}

		void exportShapeFlags(const PhysXXML::PxShape & shape)
		{
			if (shape.flags.flags != ShapeFlags::DefaultValue())
			{
				ShapeFlags e(getPhysXExporter(), shape.flags.flags);
			}
		}

		void exportName(const PhysXXML::PxShape & shape)
		{
			if (!shape.name.name.empty())
			{
				DebugName e(getPhysXExporter(), shape.name.name);
			}
		}

    private:
        static std::set<MString, MStringComp> mAttributes;
    };
    std::set<MString, MStringComp> ShapeTechnique::mAttributes;

    class ShapeExtra : public Element
    {
    public:
        ShapeExtra(PhysXExporter& exporter, const MObject& rigidBody, const MObject& shape)
            : Element(exporter, CSWC::CSW_ELEMENT_EXTRA)
        {
			if (PhysXExporter::HasExtraAttributes(shape)) {
				exportProfile(rigidBody, shape, PROFILE_MAYA);
			}
            exportProfile(rigidBody, shape, PhysXExporter::GetProfile());
            exportProfile(rigidBody, shape, PhysXExporter::GetProfileXML());
			exportProfile(rigidBody, shape, PhysXExporter::GetPhysXProfile());
        }

    private:
        void exportProfile(const MObject& rigidBody, const MObject& shape, const String& profile)
        {
            ShapeTechnique e(getPhysXExporter(), rigidBody, shape, profile);
        }
    };

    class Shape : public Element
    {
    public:
        Shape(PhysXExporter& exporter, const MObject& rigidBody, const MObject & shape)
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE)
        {
            exportHollow(shape);

            int dummy = 0;
            MString useMassOrDensity;
            DagHelper::getPlugValue(shape, ATTR_USE_MASS_OR_DENSITY, dummy, useMassOrDensity);
            if (useMassOrDensity == USE_MASS_OR_DENSITY_MASS)
            {
                exportMass(shape);
            }
            else //if (useMassOrDensity == USE_MASS_OR_DENSITY_DENSITY)
            {
                exportDensity(shape);
            }

            //no <physics_material> exported for shapes. Use rigid body's one.

            MString shapeType;
            PhysXShape::GetType(shape, shapeType);
            if (shapeType == SHAPE_TYPE_BOX)
            {
                exportBox(rigidBody, shape);
            }
            else if (shapeType == SHAPE_TYPE_SPHERE)
            {
				exportSphere(rigidBody, shape);
            }
            else if (shapeType == SHAPE_TYPE_CAPSULE)
            {
				exportCapsule(rigidBody, shape);
            }
            else if (shapeType == SHAPE_TYPE_CONVEX_HULL)
            {
				exportConvexHull(rigidBody, shape);
            }
            else if (shapeType == SHAPE_TYPE_TRIANGLE_MESH)
            {
				exportTriangleMesh(rigidBody, shape);
            }
            else if (shapeType == SHAPE_TYPE_CLOTH_SPHERES)
            {
				exportSphere(rigidBody, shape);
            }

            exportRotateTranslate(rigidBody, shape);
            exportExtra(rigidBody, shape);
        }

    private:
        void exportHollow(const MObject & shape)
        {
            //Hollow hollow(getPhysXExporter(), false);
        }

        void exportMass(const MObject & shape)
        {
            double mass = 0.0;
            DagHelper::getPlugValue(shape, ATTR_MASS, mass);
            Mass e(getPhysXExporter(), mass);
        }

        void exportDensity(const MObject & shape)
        {
            double density = 0.0;
            DagHelper::getPlugValue(shape, ATTR_DENSITY, density);
            Density e(getPhysXExporter(), density);
        }

        void exportBox(const MObject & rigidBody, const MObject & shape)
        {
            ShapeBox e(getPhysXExporter(), rigidBody, shape);
        }

		void exportSphere(const MObject & rigidBody, const MObject & shape)
        {
			ShapeSphere e(getPhysXExporter(), rigidBody, shape);
        }

		void exportCapsule(const MObject & rigidBody, const MObject & shape)
        {
			ShapeCapsule e(getPhysXExporter(), rigidBody, shape);
        }

		void exportConvexHull(const MObject & rigidBody, const MObject & shape)
        {
            // TODO PhysX: apply "inflate" attribute to convex hull geometry.
            // Note: apply inflation like done for box shape. See ShapeBox::exportHalfExtents().
            // However "inflate" attribute is limited to [0; 0.839] for some unknown reason.
			exportInstanceGeometry(rigidBody, shape, "_");
        }

		void exportTriangleMesh(const MObject & rigidBody, const MObject & shape)
        {
			exportInstanceGeometry(rigidBody, shape);
        }

		void exportInstanceGeometry(const MObject & rigidBody, const MObject & shape, const String & URISuffix = "")
        {
            // Get geometry URI
            MObject meshObject;
            PhysXShape::GetConnectedInMesh(shape, meshObject);

            if (meshObject.isNull() || !ExportOptions::exportPolygonMeshes())
            {
                // If connected mesh doesn't exist anymore then we use the PhysX shape internal geometry
                // This should rarely happen but this is possible...
                // Also use shape internal geometry if we don't export polygon meshes.
                meshObject = shape;
            }

            URI geometryURI;
            getPhysXExporter().getMeshURI(meshObject, geometryURI);
            if (URISuffix.length() > 0) {
                geometryURI.setFragment(geometryURI.getFragment() + URISuffix);
            }
            InstanceGeometry e(getPhysXExporter(), geometryURI);
        }

        void exportRotateTranslate(const MObject& rigidBody, const MObject & shape)
        {
            // Get shape local pose.
            MMatrix localPose = MMatrix::identity;
            getPhysXExporter().getShapeLocalPose(rigidBody, shape, localPose);

            // Rigidbody world pose
            MMatrix globalPose = MMatrix::identity;
            getPhysXExporter().getRigidBodyGlobalPose(rigidBody, globalPose);
            
            // Parent world pose
            MObject parent;
            MFnDagNode rigidBodyNode(rigidBody);
            parent = rigidBodyNode.parent(0);
            MDagPath parentDagPath;
            MDagPath::getAPathTo(parent, parentDagPath);
            MMatrix parentGlobalPose = parentDagPath.inclusiveMatrix();

            MMatrix DAEShapeLocalPose = localPose * globalPose * parentGlobalPose.inverse();

            int dummy = 0;
            MString shapeType;
            DagHelper::getPlugValue(shape, ATTR_SHAPE_TYPE, dummy, shapeType);
            if (shapeType == SHAPE_TYPE_CAPSULE) {
                // PhysX capsules are X axis oriented. COLLADA capsules are Y axis oriented.
                MTransformationMatrix rotation;
                rotation.rotateBy(MEulerRotation(0.0, 0.0, -M_PI / 2.0, MEulerRotation::kXYZ), MSpace::kTransform);
                MMatrix rotationMatrix = rotation.asMatrix();
                DAEShapeLocalPose = DAEShapeLocalPose * rotationMatrix;
            }

            MTransformationMatrix transform(DAEShapeLocalPose);

            MVector translation = transform.getTranslation(MSpace::kTransform);
            MVector rotatePivotTranslation = transform.rotatePivotTranslation(MSpace::kTransform);
            MPoint rotatePivot = transform.rotatePivot(MSpace::kTransform);
            MEulerRotation rotation = transform.eulerRotation();
            rotation.order = static_cast<MEulerRotation::RotationOrder>(static_cast<int>(transform.rotationOrder()) - MTransformationMatrix::kXYZ + MEulerRotation::kXYZ);

            getPhysXExporter().exportTranslation(translation, ATTR_TRANSLATE);
            getPhysXExporter().exportTranslation(rotatePivotTranslation, ATTR_ROTATE_PIVOT_TRANSLATION);
            getPhysXExporter().exportTranslation(rotatePivot, ATTR_ROTATE_PIVOT);
            getPhysXExporter().exportRotation(rotation, ATTR_ROTATE);
            getPhysXExporter().exportTranslation(rotatePivot * -1, ATTR_ROTATE_PIVOT_INVERSE);
        }

        void exportExtra(const MObject& rigidBody, const MObject & shape)
        {
            ShapeExtra e(getPhysXExporter(), rigidBody, shape);
        }
    };

    class Enabled : public Element
    {
    public:
        Enabled(PhysXExporter& exporter, bool enabled)
            : Element(exporter, CSWC::CSW_ELEMENT_ENABLED)
        {
            getStreamWriter().appendValues(enabled);
        }
    };

    class Interpenetrate : public Element
    {
    public:
        Interpenetrate(PhysXExporter& exporter, bool interpenetrate)
            : Element(exporter, CSWC::CSW_ELEMENT_INTERPENETRATE)
        {
            getStreamWriter().appendValues(interpenetrate);
        }
    };

    class Min : public Element
    {
    public:
        Min(PhysXExporter& exporter, const MVector & min)
            : Element(exporter, CSWC::CSW_ELEMENT_MIN)
        {
            for (uint i = 0; i < 3; ++i)
            {
                if (isInf(min(i))) {
                    getStreamWriter().appendValues("-INF");
                }
                else {
                    getStreamWriter().appendValues(min(i));
                }
            }
        }
    };

    class Max : public Element
    {
    public:
        Max(PhysXExporter& exporter, const MVector & max)
            : Element(exporter, CSWC::CSW_ELEMENT_MAX)
        {
            for (uint i = 0; i < 3; ++i)
            {
                if (isInf(max(i))) {
                    getStreamWriter().appendValues("INF");
                }
                else {
                    getStreamWriter().appendValues(max(i));
                }
            }
        }
    };

    class SwingConeAndTwist : public Element
    {
    public:
        SwingConeAndTwist(PhysXExporter& exporter,
            const MVector & min,
            const MVector & max)
            : Element(exporter, CSWC::CSW_ELEMENT_SWING_CONE_AND_TWIST)
        {
            exportMin(min);
            exportMax(max);
        }

    private:
        void exportMin(const MVector & min)
        {
            if (min != MVector::zero)
            {
                Min e(getPhysXExporter(), min);
            }
        }

        void exportMax(const MVector & max)
        {
            if (max != MVector::zero)
            {
                Max e(getPhysXExporter(), max);
            }
        }
    };

    class LimitsLinear : public Element
    {
    public:
        LimitsLinear(PhysXExporter& exporter,
            const MVector & min,
            const MVector & max)
            : Element(exporter, CSWC::CSW_ELEMENT_LINEAR)
        {
            exportMin(min);
            exportMax(max);
        }

    private:
        void exportMin(const MVector & min)
        {
            if (min != MVector::zero)
            {
                Min e(getPhysXExporter(), min);
            }
        }

        void exportMax(const MVector & max)
        {
            if (max != MVector::zero)
            {
                Max e(getPhysXExporter(), max);
            }
        }
    };

    class Limits : public Element
    {
    public:
        Limits(PhysXExporter& exporter,
            const MVector & swingConeAndTwistMin,
            const MVector & swingConeAndTwistMax,
            const MVector & linearMin,
            const MVector & linearMax)
            : Element(exporter, CSWC::CSW_ELEMENT_LIMITS)
        {
            exportSwingConeAndTwist(swingConeAndTwistMin, swingConeAndTwistMax);
            exportLinear(linearMin, linearMax);
        }

    private:
        void exportSwingConeAndTwist(
            const MVector & min,
            const MVector & max)
        {
            if (min != MVector::zero ||
                max != MVector::zero)
            {
                SwingConeAndTwist e(getPhysXExporter(), min, max);
            }
        }

        void exportLinear(
            const MVector & min,
            const MVector & max)
        {
            if (min != MVector::zero ||
                max != MVector::zero)
            {
                LimitsLinear e(getPhysXExporter(), min, max);
            }
        }
    };

    class Stiffness : public Element
    {
    public:
        Stiffness(PhysXExporter& exporter, double stiffness, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_STIFFNESS)
        {
            if (!sid.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            }
            getStreamWriter().appendValues(stiffness);
        }

		static double DefaultValue()
		{
			return 0.0;
		}
    };

    class Damping : public Element
    {
    public:
        Damping(PhysXExporter& exporter, double damping, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_DAMPING)
        {
            if (!sid.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            }
            getStreamWriter().appendValues(damping);
        }

		static double DefaultValue()
		{
			return 0.0;
		}
    };

    class TargetValue : public Element
    {
    public:
        TargetValue(PhysXExporter& exporter, double targetValue, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_TARGET_VALUE)
        {
            if (!sid.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            }
            getStreamWriter().appendValues(targetValue);
        }
    };

    class SpringLinear : public Element
    {
    public:
        SpringLinear(PhysXExporter& exporter,
            double stiffness,
            double damping,
            double targetValue)
            : Element(exporter, CSWC::CSW_ELEMENT_LINEAR)
        {
            exportStiffness(stiffness);
            exportDamping(damping);
            exportTargetValue(targetValue);
        }

    private:
        void exportStiffness(double stiffness)
        {
            if (stiffness != 1.0)
            {
                Stiffness e(getPhysXExporter(), stiffness);
            }
        }

        void exportDamping(double damping)
        {
            if (damping != 0.0)
            {
                Damping e(getPhysXExporter(), damping);
            }
        }

        void exportTargetValue(double targetValue)
        {
            if (targetValue != 0.0)
            {
                TargetValue e(getPhysXExporter(), targetValue);
            }
        }
    };

    class Angular : public Element
    {
    public:
        Angular(PhysXExporter& exporter,
            double stiffness,
            double damping,
            double targetValue)
            : Element(exporter, CSWC::CSW_ELEMENT_ANGULAR)
        {
            exportStiffness(stiffness);
            exportDamping(damping);
            exportTargetValue(targetValue);
        }

    private:
        void exportStiffness(double stiffness)
        {
            if (stiffness != 1.0)
            {
                Stiffness e(getPhysXExporter(), stiffness);
            }
        }

        void exportDamping(double damping)
        {
            if (damping != 0.0)
            {
                Damping e(getPhysXExporter(), damping);
            }
        }

        void exportTargetValue(double targetValue)
        {
            if (targetValue != 0.0)
            {
                TargetValue e(getPhysXExporter(), targetValue);
            }
        }
    };

    class Spring : public Element
    {
    public:
        Spring(PhysXExporter& exporter,
            double angularStiffness,
            double angularDamping,
            double angularTargetValue,
            double linearStiffness,
            double linearDamping,
            double linearTargetValue)
            : Element(exporter, CSWC::CSW_ELEMENT_SPRING)
        {
            exportAngular(angularStiffness, angularDamping, angularTargetValue);
            exportLinear(linearStiffness, linearDamping, linearTargetValue);
        }

    private:
        void exportLinear(
            double stiffness,
            double damping,
            double targetValue)
        {
            if (stiffness != 1.0 ||
                damping != 0.0 ||
                targetValue != 0.0)
            {
                SpringLinear e(getPhysXExporter(), stiffness, damping, targetValue);
            }
        }

        void exportAngular(
            double stiffness,
            double damping,
            double targetValue)
        {
            if (stiffness != 1.0 ||
                damping != 0.0 ||
                targetValue != 0.0)
            {
                Angular e(getPhysXExporter(), stiffness, damping, targetValue);
            }
        }
    };

    class RigidBodyTechniqueCommon : public Element
    {
    public:
        RigidBodyTechniqueCommon(PhysXExporter& exporter, const MObject & rigidBody)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE_COMMON)
        {
            exportDynamic(rigidBody);
            exportMass(rigidBody);
            exportMassFrame(rigidBody);
            exportInertia(rigidBody);
            exportPhysicsMaterial(rigidBody);
            exportShapes(rigidBody);
        }

    private:
        void exportDynamic(const MObject & rigidBody)
        {
            int dummy = 0;
            MString simulationType;
            DagHelper::getPlugValue(rigidBody, ATTR_SIMULATION_TYPE, dummy, simulationType);
            Dynamic e(getPhysXExporter(), simulationType != SIMULATION_TYPE_STATIC);
        }

        void exportMass(const MObject & rigidBody)
        {
            double mass = getPhysXExporter().GetRigidBodyMass(rigidBody);
            Mass e(getPhysXExporter(), mass);
        }

        void exportMassFrame(const MObject & rigidBody)
        {
            int dummy = 0;
            MString centerOfMassMode;
            DagHelper::getPlugValue(rigidBody, ATTR_CENTER_OF_MASS_MODE, dummy, centerOfMassMode);

            MString overrideMassOrDensityStr;
            DagHelper::getPlugValue(rigidBody, ATTR_OVERRIDE_MASS_OR_DENSITY, dummy, overrideMassOrDensityStr);
            bool overrideMassOrDensity = overrideMassOrDensityStr != OVERRIDE_MASS_OR_DENSITY_DISABLED;

            MVector translation = MVector::zero;
            if (centerOfMassMode == CENTER_OF_MASS_MODE_CALCULATE_FROM_SHAPES)
            {
                std::vector<MObject> physicsShapes;
                getPhysXExporter().GetRigidBodyShapes(rigidBody, physicsShapes);

                double totalFactor = 0.0;
                for (size_t i = 0; i < physicsShapes.size(); ++i)
                {
                    const MObject & shape = physicsShapes[i];
                    MDagPath shapeDagPath;
                    MDagPath::getAPathTo(shape, shapeDagPath);
                    MFnTransform shapeTransform(shapeDagPath.transform());
                    double shapeFactor = overrideMassOrDensity ? getPhysXExporter().GetShapeVolume(shape) : getPhysXExporter().GetShapeMass(shape);
                    translation += shapeTransform.getTranslation(MSpace::kObject) * shapeFactor;
                    totalFactor += shapeFactor;
                }
                if (totalFactor > 0.0)
                {
                    translation /= totalFactor;
                }
            }
            else //if (centerOfMassMode == CENTER_OF_MASS_MODE_MANUAL_OVERRIDE)
            {
                DagHelper::getPlugValue(rigidBody, ATTR_CENTER_OF_MASS_OVERRIDE, translation);
            }

            if (translation != MVector::zero)
            {
                MassFrame e(getPhysXExporter(), translation, ATTR_TRANSLATE);
            }
        }

        void exportInertia(const MObject & rigidBody)
        {
            /*
            Bounding box inertia matrix =

            m(b�+c�)/12     0               0
            0               m(a�+c�)/12     0
            0               0               m(a�+b�)/12

            m -> mass
            a, b, c -> edge lengths
            */

            MVector bb = MVector::zero;
            DagHelper::getPlugValue(rigidBody, ATTR_BOUNDING_BOX_SIZE, bb);

            // Convert to DAE unit
            bb.x = MDistance::internalToUI(bb.x);
            bb.y = MDistance::internalToUI(bb.y);
            bb.z = MDistance::internalToUI(bb.z);

            double mass = getPhysXExporter().GetRigidBodyMass(rigidBody);

            MVector inertiaMatrixDiagonal = MVector::zero;
            inertiaMatrixDiagonal.x = mass * (bb.y * bb.y + bb.z * bb.z) / 12.0;
            inertiaMatrixDiagonal.y = mass * (bb.x * bb.x + bb.z * bb.z) / 12.0;
            inertiaMatrixDiagonal.z = mass * (bb.x * bb.x + bb.y * bb.y) / 12.0;

            Inertia e(getPhysXExporter(), inertiaMatrixDiagonal);
        }

        void exportPhysicsMaterial(const MObject & rigidBody)
        {
            PhysicsMaterial e(getPhysXExporter(), rigidBody);
        }

        void exportShapes(const MObject & rigidBody)
        {
            std::vector<MObject> physicsShapes;
            getPhysXExporter().GetRigidBodyShapes(rigidBody, physicsShapes);

            for (size_t i = 0; i < physicsShapes.size(); ++i)
            {
                const MObject & shape = physicsShapes[i];

                Shape e(getPhysXExporter(), rigidBody, shape);
            }
        }
    };

    class RigidConstraintTechniqueCommon : public Element
    {
    public:
        RigidConstraintTechniqueCommon(PhysXExporter& exporter, const MObject & rigidConstraint)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE_COMMON)
        {
            exportEnabled(rigidConstraint);
            exportInterpenetrate(rigidConstraint);
            exportLimits(rigidConstraint);
            exportSpring(rigidConstraint);
        }

    private:
        void exportEnabled(const MObject & rigidConstraint)
        {
            bool isEnabled = true;
            DagHelper::getPlugValue(rigidConstraint, ATTR_CONSTRAIN, isEnabled);
            if (!isEnabled)
            {
                Enabled e(getPhysXExporter(), isEnabled);
            }
        }

        void exportInterpenetrate(const MObject & rigidConstraint)
        {
            bool interpenetrate = false;
            DagHelper::getPlugValue(rigidConstraint, ATTR_INTERPENETRATE, interpenetrate);
            if (interpenetrate)
            {
                Interpenetrate e(getPhysXExporter(), interpenetrate);
            }
        }

        void exportLimits(const MObject & rigidConstraint)
        {
            MVector swingConeAndTwistMin, swingConeAndTwistMax, linearMin, linearMax;
            PhysXRigidConstraint::GetSwingConeAndTwistMinLimit(rigidConstraint, swingConeAndTwistMin);
            PhysXRigidConstraint::GetSwingConeAndTwistMaxLimit(rigidConstraint, swingConeAndTwistMax);
            PhysXRigidConstraint::GetLinearMinLimit(rigidConstraint, linearMin);
            PhysXRigidConstraint::GetLinearMaxLimit(rigidConstraint, linearMax);
            if (swingConeAndTwistMin != MVector::zero ||
                swingConeAndTwistMax != MVector::zero ||
                linearMin != MVector::zero ||
                linearMax != MVector::zero)
            {
                Limits e(getPhysXExporter(),
                    swingConeAndTwistMin,
                    swingConeAndTwistMax,
                    linearMin,
                    linearMax);
            }
        }

        void exportSpring(const MObject & rigidConstraint)
        {
            double linearStiffness = 1.0;
            double linearDamping = 0.0;
            double linearTargetValue = 0.0;
            double angularStiffness = 1.0;
            double angularDamping = 0.0;
            double angularTargetValue = 0.0;
            PhysXRigidConstraint::GetSpringLinearStiffness(rigidConstraint, linearStiffness);
            PhysXRigidConstraint::GetSpringLinearDamping(rigidConstraint, linearDamping);
            PhysXRigidConstraint::GetSpringLinearTargetValue(rigidConstraint, linearTargetValue);
            PhysXRigidConstraint::GetSpringAngularStiffness(rigidConstraint, angularStiffness);
            PhysXRigidConstraint::GetSpringAngularDamping(rigidConstraint, angularDamping);
            PhysXRigidConstraint::GetSpringAngularTargetValue(rigidConstraint, angularTargetValue);
            if (linearStiffness != 1.0 ||
                linearDamping != 0.0 ||
                linearTargetValue != 0.0 ||
                angularStiffness != 1.0 ||
                angularDamping != 0.0 ||
                angularTargetValue != 0.0)
            {
                Spring e(getPhysXExporter(),
                    angularStiffness,
                    angularDamping,
                    angularTargetValue,
                    linearStiffness,
                    linearDamping,
                    linearTargetValue);
            }
        }
    };

	class ActorFlags : public Element
	{
	public:
		ActorFlags(PhysXExporter & exporter, const Flags<PhysXXML::ActorFlags::FlagEnum> & flags)
			: Element(exporter, CSWC::CSW_ELEMENT_ACTOR_FLAGS)
		{
			getStreamWriter().appendValues(PhysXExporter::ActorFlagsToCOLLADA(flags));
		}

		static Flags<PhysXXML::ActorFlags::FlagEnum> DefaultValue()
		{
			return Flags<PhysXXML::ActorFlags::FlagEnum>(PhysXXML::ActorFlags::FlagEnum::Visualization);
		}
	};

	class DominanceGroup : public Element
	{
	public:
		DominanceGroup(PhysXExporter & exporter, int dominanceGroup)
			: Element(exporter, CSWC::CSW_ELEMENT_DOMINANCE_GROUP)
		{
			getStreamWriter().appendValues(dominanceGroup);
		}

		static int DefaultValue()
		{
			return 0;
		}
	};
	
	class OwnerClient : public Element
	{
	public:
		OwnerClient(PhysXExporter & exporter, int ownerClient)
			: Element(exporter, CSWC::CSW_ELEMENT_OWNER_CLIENT)
		{
			getStreamWriter().appendValues(ownerClient);
		}
	};

	class CMassLocalPose : public Element
	{
	public:
		CMassLocalPose(PhysXExporter & exporter, const MMatrix & cMassLocalPose)
			: Element(exporter, CSWC::CSW_ELEMENT_CMASS_LOCAL_POSE)
		{
			getStreamWriter().appendValues(cMassLocalPose.matrix);
		}
	};

	class RigidBodyFlags : public Element
	{
	public:
		RigidBodyFlags(PhysXExporter & exporter, const Flags<PhysXXML::RigidBodyFlags::FlagEnum> & flags)
			: Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY_FLAGS)
		{
			getStreamWriter().appendValues(PhysXExporter::RigidBodyFlagsToCOLLADA(flags));
		}

		static Flags<PhysXXML::RigidBodyFlags::FlagEnum> DefaultValue()
		{
			return Flags<PhysXXML::RigidBodyFlags::FlagEnum>();
		}
	};
	
	class MinCCDAdvanceCoefficient : public Element
	{
	public:
		MinCCDAdvanceCoefficient(PhysXExporter & exporter, double minCCDAdvanceCoefficient)
			: Element(exporter, CSWC::CSW_ELEMENT_MIN_CCD_ADVANCE_COEFFICIENT)
		{
			getStreamWriter().appendValues(minCCDAdvanceCoefficient);
		}
	};

	class MaxDepenetrationVelocity : public Element
	{
	public:
		MaxDepenetrationVelocity(PhysXExporter & exporter, double maxDepenetrationVelocity)
			: Element(exporter, CSWC::CSW_ELEMENT_MAX_DEPENETRATION_VELOCITY)
		{
			getStreamWriter().appendValues(maxDepenetrationVelocity);
		}
	};

	class LinearDamping : public Element
	{
	public:
		LinearDamping(PhysXExporter & exporter, double linearDamping)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_DAMPING)
		{
			getStreamWriter().appendValues(linearDamping);
		}
	};

	class AngularDamping : public Element
	{
	public:
		AngularDamping(PhysXExporter & exporter, double angularDamping)
			: Element(exporter, CSWC::CSW_ELEMENT_ANGULAR_DAMPING)
		{
			getStreamWriter().appendValues(angularDamping);
		}
	};

	class MaxAngularVelocity : public Element
	{
	public:
		MaxAngularVelocity(PhysXExporter & exporter, double maxAngularDamping)
			: Element(exporter, CSWC::CSW_ELEMENT_MAX_ANGULAR_VELOCITY)
		{
			getStreamWriter().appendValues(maxAngularDamping);
		}
	};

	class SleepThreshold : public Element
	{
	public:
		SleepThreshold(PhysXExporter & exporter, double sleepThreshold)
			: Element(exporter, CSWC::CSW_ELEMENT_SLEEP_THRESHOLD)
		{
			getStreamWriter().appendValues(sleepThreshold);
		}
	};

	class StabilizationThreshold : public Element
	{
	public:
		StabilizationThreshold(PhysXExporter & exporter, double stabilizationThreshold)
			: Element(exporter, CSWC::CSW_ELEMENT_STABILIZATION_THRESHOLD)
		{
			getStreamWriter().appendValues(stabilizationThreshold);
		}
	};

	class WakeCounter : public Element
	{
	public:
		WakeCounter(PhysXExporter & exporter, double wakeCounter)
			: Element(exporter, CSWC::CSW_ELEMENT_WAKE_COUNTER)
		{
			getStreamWriter().appendValues(wakeCounter);
		}
	};

	class MinPositionIters : public Element
	{
	public:
		MinPositionIters(PhysXExporter & exporter, int minPositionIters)
			: Element(exporter, CSWC::CSW_ELEMENT_MIN_POSITION_ITERS)
		{
			getStreamWriter().appendValues(minPositionIters);
		}
	};

	class MinVelocityIters : public Element
	{
	public:
		MinVelocityIters(PhysXExporter & exporter, int minVelocityIters)
			: Element(exporter, CSWC::CSW_ELEMENT_MIN_VELOCITY_ITERS)
		{
			getStreamWriter().appendValues(minVelocityIters);
		}
	};

	class ContactReportThreshold : public Element
	{
	public:
		ContactReportThreshold(PhysXExporter & exporter, double contactReportThreshold)
			: Element(exporter, CSWC::CSW_ELEMENT_CONTACT_REPORT_THRESHOLD)
		{
			getStreamWriter().appendValues(contactReportThreshold);
		}
	};

    class RigidBodyTechnique : public Element
    {
    public:
        RigidBodyTechnique(PhysXExporter& exporter, const MObject & rigidBody, const String & profile)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, profile);
            
            if (profile == PhysXExporter::GetProfile()) {
                exporter.exportAttributes(rigidBody, GetAttributes());
            }
            else if (profile == PhysXExporter::GetProfileXML()) {
                exporter.exportRigidBodyPhysXXML(rigidBody);
            }
			else if (profile == PROFILE_MAYA) {
				exporter.exportExtraAttributes(rigidBody);
			}
			else if (profile == PhysXExporter::GetPhysXProfile()) {
				PhysXXML::PxRigidDynamic* pxRigidDynamic = exporter.findPxRigidDynamic(rigidBody);
				PhysXXML::PxRigidStatic* pxRigidStatic = exporter.findPxRigidStatic(rigidBody);
				PhysXXML::PxRigidBody* pxRigidBody = pxRigidDynamic ? static_cast<PhysXXML::PxRigidBody*>(pxRigidDynamic) : static_cast<PhysXXML::PxRigidBody*>(pxRigidStatic);
				if (pxRigidBody)
				{
					exportActorFlags(*pxRigidBody);
					exportDominanceGroup(*pxRigidBody);
					exportOwnerClient(*pxRigidBody);
				}
				if (pxRigidDynamic)
				{
					exportCMassLocalPose(*pxRigidDynamic);
					exportRigidBodyFlags(*pxRigidDynamic);
					exportMinCCDAdvanceCoefficient(*pxRigidDynamic);
					exportMaxDepenetrationVelocity(*pxRigidDynamic);
					exportLinearDamping(*pxRigidDynamic);
					exportAngularDamping(*pxRigidDynamic);
					exportMaxAngularVelocity(*pxRigidDynamic);
					exportSleepThreshold(*pxRigidDynamic);
					exportStabilizationThreshold(*pxRigidDynamic);
					exportWakeCounter(*pxRigidDynamic);
					exportMinPositionIters(*pxRigidDynamic);
					exportMinVelocityIters(*pxRigidDynamic);
					exportContactReportThreshold(*pxRigidDynamic);
				}
			}
        }

    private:
        static const std::set<MString, MStringComp>& GetAttributes()
        {
            if (mAttributes.size() == 0)
            {
                // Attributes we want to export in <extra> section
                mAttributes.insert(ATTR_SIMULATION_TYPE);
                mAttributes.insert(ATTR_SWITCH_TO_DYNAMIC);
                mAttributes.insert(ATTR_SWITCH_AT_FRAME);
                mAttributes.insert(ATTR_ENABLE_GRAVITY);
                mAttributes.insert(ATTR_FORCE_TO_SLEEP);
                mAttributes.insert(ATTR_OVERRIDE_MASS_OR_DENSITY);
                mAttributes.insert(ATTR_MASS);
                mAttributes.insert(ATTR_DENSITY);
                mAttributes.insert(ATTR_BOUNCINESS);
                mAttributes.insert(ATTR_STATIC_FRICTION);
                mAttributes.insert(ATTR_DYNAMIC_FRICTION);
                mAttributes.insert(ATTR_OVERRIDE_GLOBAL_ITERATION_COUNT);
                mAttributes.insert(ATTR_POSITION_ITERATION_COUNT);
                mAttributes.insert(ATTR_VELOCITY_ITERATION_COUNT);
                mAttributes.insert(ATTR_CONTACT_SHELL_OVERRIDE);
                mAttributes.insert(ATTR_CONTACT_SHELL_DEPTH);
                mAttributes.insert(ATTR_CONTACT_SHELL_OFFSET);
                mAttributes.insert(ATTR_SLEEP_THRESHOLDS_OVERRIDE);
                mAttributes.insert(ATTR_SLEEP_ENERGY_THRESHOLD);
                mAttributes.insert(ATTR_LINEAR_DAMPING);
                mAttributes.insert(ATTR_ANGULAR_DAMPING);
                mAttributes.insert(ATTR_CENTER_OF_MASS_MODE);
                mAttributes.insert(ATTR_CENTER_OF_MASS_OVERRIDE);
                mAttributes.insert(ATTR_INITIAL_VELOCITY);
                mAttributes.insert(ATTR_INITIAL_SPIN);
                mAttributes.insert(ATTR_INITIAL_POSITION);
                mAttributes.insert(ATTR_INITIAL_ORIENTATION);
                mAttributes.insert(ATTR_ENABLE_CCD);
                mAttributes.insert(ATTR_CCD_MOTION_THRESHOLD);
            }
            return mAttributes;
        }

		void exportActorFlags(const PhysXXML::PxRigidBody & pxRigidBody)
		{
			if (pxRigidBody.actorFlags.actorFlags != ActorFlags::DefaultValue())
			{
				ActorFlags e(getPhysXExporter(), pxRigidBody.actorFlags.actorFlags);
			}
		}

		void exportDominanceGroup(const PhysXXML::PxRigidBody & pxRigidBody)
		{
			if (pxRigidBody.dominanceGroup.dominanceGroup != DominanceGroup::DefaultValue())
			{
				DominanceGroup e(getPhysXExporter(), pxRigidBody.dominanceGroup.dominanceGroup);
			}
		}

		void exportOwnerClient(const PhysXXML::PxRigidBody & pxRigidBody)
		{
			if (pxRigidBody.ownerClient.ownerClient != 0)
			{
				OwnerClient e(getPhysXExporter(), pxRigidBody.ownerClient.ownerClient);
			}
		}

		void exportCMassLocalPose(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.cMassLocalPose.rotation != MQuaternion::identity ||
				pxRigidDynamic.cMassLocalPose.translation != MVector::zero)
			{
				MTransformationMatrix tm;

				tm.setRotationQuaternion(
					pxRigidDynamic.cMassLocalPose.rotation.x,
					pxRigidDynamic.cMassLocalPose.rotation.y,
					pxRigidDynamic.cMassLocalPose.rotation.z,
					pxRigidDynamic.cMassLocalPose.rotation.w);

				tm.setTranslation(MVector(
					MDistance::uiToInternal(pxRigidDynamic.cMassLocalPose.translation.x),
					MDistance::uiToInternal(pxRigidDynamic.cMassLocalPose.translation.y),
					MDistance::uiToInternal(pxRigidDynamic.cMassLocalPose.translation.z)
					), MSpace::kTransform);

				CMassLocalPose e(getPhysXExporter(), tm.asMatrix());
			}
		}

		void exportRigidBodyFlags(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.rigidBodyFlags.rigidBodyFlags != RigidBodyFlags::DefaultValue())
			{
				RigidBodyFlags e(getPhysXExporter(), pxRigidDynamic.rigidBodyFlags.rigidBodyFlags);
			}
		}

		void exportMinCCDAdvanceCoefficient(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.minCCDAdvanceCoefficient.minCCDAdvanceCoefficient != 0.15)
			{
				MinCCDAdvanceCoefficient e(getPhysXExporter(), pxRigidDynamic.minCCDAdvanceCoefficient.minCCDAdvanceCoefficient);
			}
		}

		void exportMaxDepenetrationVelocity(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.maxDepenetrationVelocity.maxDepenetrationVelocity != infinite())
			{
				MaxDepenetrationVelocity e(getPhysXExporter(), pxRigidDynamic.maxDepenetrationVelocity.maxDepenetrationVelocity);
			}
		}

		void exportLinearDamping(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.linearDamping.linearDamping != 0.0)
			{
				LinearDamping e(getPhysXExporter(), pxRigidDynamic.linearDamping.linearDamping);
			}
		}

		void exportAngularDamping(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.angularDamping.angularDamping != 0.0)
			{
				AngularDamping e(getPhysXExporter(), pxRigidDynamic.angularDamping.angularDamping);
			}
		}

		void exportMaxAngularVelocity(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.maxAngularVelocity.maxAngularVelocity != 7.0)
			{
				MaxAngularVelocity e(getPhysXExporter(), pxRigidDynamic.maxAngularVelocity.maxAngularVelocity);
			}
		}

		void exportSleepThreshold(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.sleepThreshold.sleepThreshold != 5e-5)
			{
				SleepThreshold e(getPhysXExporter(), pxRigidDynamic.sleepThreshold.sleepThreshold);
			}
		}

		void exportStabilizationThreshold(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.stabilizationThreshold.stabilizationThreshold != 1e-5)
			{
				StabilizationThreshold e(getPhysXExporter(), pxRigidDynamic.stabilizationThreshold.stabilizationThreshold);
			}
		}

		void exportWakeCounter(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.wakeCounter.wakeCounter != 0.4)
			{
				WakeCounter e(getPhysXExporter(), pxRigidDynamic.wakeCounter.wakeCounter);
			}
		}

		void exportMinPositionIters(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.solverIterationCounts.minPositionIters.minPositionIters != 4)
			{
				MinPositionIters e(getPhysXExporter(), pxRigidDynamic.solverIterationCounts.minPositionIters.minPositionIters);
			}
		}

		void exportMinVelocityIters(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.solverIterationCounts.minVelocityIters.minVelocityIters != 1)
			{
				MinVelocityIters e(getPhysXExporter(), pxRigidDynamic.solverIterationCounts.minVelocityIters.minVelocityIters);
			}
		}

		void exportContactReportThreshold(const PhysXXML::PxRigidDynamic & pxRigidDynamic)
		{
			if (pxRigidDynamic.contactReportThreshold.contactReportThreshold != infinite())
			{
				ContactReportThreshold e(getPhysXExporter(), pxRigidDynamic.contactReportThreshold.contactReportThreshold);
			}
		}

    private:
        static std::set<MString, MStringComp> mAttributes;
    };
    std::set<MString, MStringComp> RigidBodyTechnique::mAttributes;

    class RigidBodyExtra : public Element
    {
    public:
        RigidBodyExtra(PhysXExporter& exporter, const MObject & rigidBody)
            : Element(exporter, CSWC::CSW_ELEMENT_EXTRA)
        {
			if (PhysXExporter::HasExtraAttributes(rigidBody)) {
				exportProfile(rigidBody, PROFILE_MAYA);
			}
            exportProfile(rigidBody, PhysXExporter::GetProfile());
            exportProfile(rigidBody, PhysXExporter::GetProfileXML());
			exportProfile(rigidBody, PhysXExporter::GetPhysXProfile());
        }

    private:
        void exportProfile(const MObject& rigidBody, const String& profile)
        {
            RigidBodyTechnique e(getPhysXExporter(), rigidBody, profile);
        }
    };

    class RigidBody : public Element
    {
    public:
        RigidBody(PhysXExporter& exporter, const MObject & rigidBody, const String & sid, const String & name = "")
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_BODY)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            if (name.length() > 0) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
            }

            exportTechniqueCommon(rigidBody);
            exportExtra(rigidBody);
        }

    private:
        void exportTechniqueCommon(const MObject & rigidBody)
        {
            RigidBodyTechniqueCommon e(getPhysXExporter(), rigidBody);
        }

        void exportExtra(const MObject & rigidBody)
        {
            RigidBodyExtra e(getPhysXExporter(), rigidBody);
        }
    };

    class RefAttachment : public Element
    {
    public:
        RefAttachment(PhysXExporter& exporter, const MObject & rigidConstraint, const URI & rigidBodyURI)
            : Element(exporter, CSWC::CSW_ELEMENT_REF_ATTACHMENT)
        {
            if (rigidBodyURI.isValid()) {
                getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_RIGID_BODY, rigidBodyURI);
            }
            exportRotateTranslate(rigidConstraint);
        }

    private:
        void exportRotateTranslate(const MObject & rigidConstraint)
        {
			PhysXXML::PxD6Joint* pxJoint = getPhysXExporter().findPxD6Joint(rigidConstraint);
			if (!pxJoint)
				return;

			MVector translation = pxJoint->localPose.eActor0.translation;
			MEulerRotation rotation;
			rotation = pxJoint->localPose.eActor0.rotation;

			getPhysXExporter().exportTranslationWithoutConversion(translation, ATTR_TRANSLATE);
			getPhysXExporter().exportRotation(rotation, ATTR_ROTATE);
        }
    };

    class Attachment : public Element
    {
    public:
        Attachment(PhysXExporter& exporter, const MObject & rigidConstraint, const URI & rigidBodyURI)
            : Element(exporter, CSWC::CSW_ELEMENT_ATTACHMENT)
        {
            if (rigidBodyURI.isValid()) {
                getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_RIGID_BODY, rigidBodyURI);
            }
            exportRotateTranslate(rigidConstraint);
        }

    private:
        void exportRotateTranslate(const MObject & rigidConstraint)
        {
			PhysXXML::PxD6Joint* pxJoint = getPhysXExporter().findPxD6Joint(rigidConstraint);
			MVector translation = pxJoint->localPose.eActor1.translation;
			MEulerRotation rotation;
			rotation = pxJoint->localPose.eActor1.rotation;

			getPhysXExporter().exportTranslationWithoutConversion(translation, ATTR_TRANSLATE);
			getPhysXExporter().exportRotation(rotation, ATTR_ROTATE);
        }
    };

	class BreakForce : public Element
	{
	public:
		BreakForce(PhysXExporter & exporter, double breakForce)
			: Element(exporter, CSWC::CSW_ELEMENT_BREAK_FORCE)
		{
			getStreamWriter().appendValues(breakForce);
		}

		static double DefaultValue()
		{
			return infinite();
		}
	};
	
	class BreakTorque : public Element
	{
	public:
		BreakTorque(PhysXExporter & exporter, double breakTorque)
			: Element(exporter, CSWC::CSW_ELEMENT_BREAK_TORQUE)
		{
			getStreamWriter().appendValues(breakTorque);
		}

		static double DefaultValue()
		{
			return infinite();
		}
	};

	class ConstraintFlags : public Element
	{
	public:
		ConstraintFlags(PhysXExporter & exporter, const Flags<PhysXXML::ConstraintFlags::FlagEnum> & flags)
			: Element(exporter, CSWC::CSW_ELEMENT_CONSTRAINT_FLAGS)
		{
			getStreamWriter().appendValues(PhysXExporter::ConstraintFlagsToCOLLADA(flags));
		}

		static Flags<PhysXXML::ConstraintFlags::FlagEnum> DefaultValue()
		{
			return Flags<PhysXXML::ConstraintFlags::FlagEnum>();
		}
	};

	class InvMassScale0 : public Element
	{
	public:
		InvMassScale0(PhysXExporter & exporter, double invMassScale0)
			: Element(exporter, CSWC::CSW_ELEMENT_INV_MASS_SCALE_0)
		{
			getStreamWriter().appendValues(invMassScale0);
		}

		static double DefaultValue()
		{
			return 1.0;
		}
	};

	class InvInertiaScale0 : public Element
	{
	public:
		InvInertiaScale0(PhysXExporter & exporter, double invInertiaScale0)
			: Element(exporter, CSWC::CSW_ELEMENT_INV_INERTIA_SCALE_0)
		{
			getStreamWriter().appendValues(invInertiaScale0);
		}

		static double DefaultValue()
		{
			return 1.0;
		}
	};

	class InvMassScale1 : public Element
	{
	public:
		InvMassScale1(PhysXExporter & exporter, double invMassScale1)
			: Element(exporter, CSWC::CSW_ELEMENT_INV_MASS_SCALE_1)
		{
			getStreamWriter().appendValues(invMassScale1);
		}

		static double DefaultValue()
		{
			return 1.0;
		}
	};

	class InvInertiaScale1 : public Element
	{
	public:
		InvInertiaScale1(PhysXExporter & exporter, double invInertiaScale1)
			: Element(exporter, CSWC::CSW_ELEMENT_INV_INERTIA_SCALE_1)
		{
			getStreamWriter().appendValues(invInertiaScale1);
		}

		static double DefaultValue()
		{
			return 1.0;
		}
	};

	class ProjectionLinearTolerance : public Element
	{
	public:
		ProjectionLinearTolerance(PhysXExporter & exporter, double projectionLinearTolerance)
			: Element(exporter, CSWC::CSW_ELEMENT_PROJECTION_LINEAR_TOLERANCE)
		{
			getStreamWriter().appendValues(projectionLinearTolerance);
		}

		static double DefaultValue()
		{
			return 1e10;
		}
	};

	class ProjectionAngularTolerance : public Element
	{
	public:
		ProjectionAngularTolerance(PhysXExporter & exporter, double projectionAngularTolerance)
			: Element(exporter, CSWC::CSW_ELEMENT_PROJECTION_ANGULAR_TOLERANCE)
		{
			getStreamWriter().appendValues(COLLADABU::Math::Utils::radToDeg(projectionAngularTolerance));
		}

		static double DefaultValue()
		{
			return M_PI;
		}
	};

	class BounceThreshold : public Element
	{
	public:
		BounceThreshold(PhysXExporter & exporter, double bounceThreshold)
			: Element(exporter, CSWC::CSW_ELEMENT_BOUNCE_THRESHOLD)
		{
			getStreamWriter().appendValues(bounceThreshold);
		}

		static double DefaultValue()
		{
			return 0.0;
		}
	};

	class ContactDistance : public Element
	{
	public:
		ContactDistance(PhysXExporter & exporter, double contactDistance)
			: Element(exporter, CSWC::CSW_ELEMENT_CONTACT_DISTANCE)
		{
			getStreamWriter().appendValues(contactDistance);
		}

		static double DefaultValue()
		{
			return 0.0;
		}
	};

	class LimitsLinearExtra : public Element
	{
	public:
		LimitsLinearExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_EXTRA)
		{
			exportRestitution(joint);
			exportBounceThreshold(joint);
			exportContactDistance(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.linearLimit.restitution.restitution == Restitution::DefaultValue() &&
				joint.linearLimit.bounceThreshold.bounceThreshold == BounceThreshold::DefaultValue() &&
				joint.linearLimit.contactDistance.contactDistance == ContactDistance::DefaultValue();
		}

	private:
		void exportRestitution(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.linearLimit.restitution.restitution != Restitution::DefaultValue())
			{
				Restitution e(getPhysXExporter(), joint.linearLimit.restitution.restitution);
			}
		}

		void exportBounceThreshold(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.linearLimit.bounceThreshold.bounceThreshold != BounceThreshold::DefaultValue())
			{
				BounceThreshold e(getPhysXExporter(), joint.linearLimit.bounceThreshold.bounceThreshold);
			}
		}

		void exportContactDistance(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.linearLimit.contactDistance.contactDistance != ContactDistance::DefaultValue())
			{
				ContactDistance e(getPhysXExporter(), joint.linearLimit.contactDistance.contactDistance);
			}
		}
	};

	class Restitution2 : public Element
	{
	public:
		Restitution2(PhysXExporter & exporter, double swingRestitution, double twistRestitution)
			: Element(exporter, CSWC::CSW_ELEMENT_RESTITUTION)
		{
			getStreamWriter().appendValues(swingRestitution, twistRestitution);
		}

		static bool AreDefaultValues(double swingRestitution, double twistRestitution)
		{
			return swingRestitution == 0.0 && twistRestitution == 0.0;
		}
	};

	class BounceThreshold2 : public Element
	{
	public:
		BounceThreshold2(PhysXExporter & exporter, double swingBounceThreshold, double twistBounceThreshold)
			: Element(exporter, CSWC::CSW_ELEMENT_BOUNCE_THRESHOLD)
		{
			getStreamWriter().appendValues(swingBounceThreshold, twistBounceThreshold);
		}

		static bool AreDefaultValues(double swingBounceThreshold, double twistBounceThreshold)
		{
			return swingBounceThreshold == 0.0 && twistBounceThreshold == 0.0;
		}
	};

	class ContactDistance2 : public Element
	{
	public:
		ContactDistance2(PhysXExporter & exporter, double swingContactDistance, double twistContactDistance)
			: Element(exporter, CSWC::CSW_ELEMENT_CONTACT_DISTANCE)
		{
			getStreamWriter().appendValues(swingContactDistance, twistContactDistance);
		}

		static bool AreDefaultValues(double swingContactDistance, double twistContactDistance)
		{
			return swingContactDistance == 0.0 && twistContactDistance == 0.0;
		}
	};

	class SwingConeAndTwistExtra : public Element
	{
	public:
		SwingConeAndTwistExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_SWING_CONE_AND_TWIST_EXTRA)
		{
			exportRestitution(joint);
			exportBounceThreshold(joint);
			exportContactDistance(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return Restitution2::AreDefaultValues(joint.swingLimit.restitution.restitution, joint.twistLimit.restitution.restitution) &&
				BounceThreshold2::AreDefaultValues(joint.swingLimit.bounceThreshold.bounceThreshold, joint.twistLimit.bounceThreshold.bounceThreshold) &&
				ContactDistance2::AreDefaultValues(joint.swingLimit.contactDistance.contactDistance, joint.twistLimit.contactDistance.contactDistance);
		}

	private:
		void exportRestitution(const PhysXXML::PxD6Joint & joint)
		{
			if (!Restitution2::AreDefaultValues(joint.swingLimit.restitution.restitution, joint.twistLimit.restitution.restitution))
			{
				Restitution2 e(getPhysXExporter(), joint.swingLimit.restitution.restitution, joint.twistLimit.restitution.restitution);
			}
		}

		void exportBounceThreshold(const PhysXXML::PxD6Joint & joint)
		{
			if (!BounceThreshold2::AreDefaultValues(joint.swingLimit.bounceThreshold.bounceThreshold, joint.twistLimit.bounceThreshold.bounceThreshold))
			{
				BounceThreshold2 e(getPhysXExporter(), joint.swingLimit.bounceThreshold.bounceThreshold, joint.twistLimit.bounceThreshold.bounceThreshold);
			}
		}

		void exportContactDistance(const PhysXXML::PxD6Joint & joint)
		{
			if (!ContactDistance2::AreDefaultValues(joint.swingLimit.contactDistance.contactDistance, joint.twistLimit.contactDistance.contactDistance))
			{
				ContactDistance2 e(getPhysXExporter(), joint.swingLimit.contactDistance.contactDistance, joint.twistLimit.contactDistance.contactDistance);
			}
		}
	};

	class LimitsExtra : public Element
	{
	public:
		LimitsExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LIMITS_EXTRA)
		{
			exportLinearExtra(joint);
			exportSwingConeAndTwistExtra(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return LimitsLinearExtra::HasDefaultValues(joint) &&
				SwingConeAndTwistExtra::HasDefaultValues(joint);
		}

	private:
		void exportLinearExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!LimitsLinearExtra::HasDefaultValues(joint))
			{
				LimitsLinearExtra e(getPhysXExporter(), joint);
			}
		}

		void exportSwingConeAndTwistExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!SwingConeAndTwistExtra::HasDefaultValues(joint))
			{
				SwingConeAndTwistExtra e(getPhysXExporter(), joint);
			}
		}
	};

	class TargetValueExtra : public Element
	{
	public:
		TargetValueExtra(PhysXExporter & exporter, double a, double b)
			: Element(exporter, CSWC::CSW_ELEMENT_TARGET_VALUE_EXTRA)
		{
			getStreamWriter().appendValues(a, b);
		}

		static bool AreDefaultValues(double a, double b)
		{
			return a == 0.0 && b == 0.0;
		}
	};

	class SpringLinearExtra : public Element
	{
	public:
		SpringLinearExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_EXTRA)
		{
			exportTargetValueExtra(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return TargetValueExtra::AreDefaultValues(joint.drivePosition.translation.y, joint.drivePosition.translation.z);
		}

	private:
		void exportTargetValueExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!TargetValueExtra::AreDefaultValues(joint.drivePosition.translation.y, joint.drivePosition.translation.z))
			{
				TargetValueExtra e(getPhysXExporter(), joint.drivePosition.translation.y, joint.drivePosition.translation.z);
			}
		}
	};

	class SpringAngularExtra : public Element
	{
	public:
		SpringAngularExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_ANGULAR_EXTRA)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportTargetValueExtra(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			MEulerRotation euler = joint.drivePosition.rotation.asEulerRotation();
			return joint.twistLimit.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.twistLimit.damping.damping == Damping::DefaultValue() &&
				TargetValueExtra::AreDefaultValues(euler.y, euler.z);
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.twistLimit.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.twistLimit.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.twistLimit.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.twistLimit.damping.damping);
			}
		}

		void exportTargetValueExtra(const PhysXXML::PxD6Joint & joint)
		{
			MEulerRotation euler = joint.drivePosition.rotation.asEulerRotation();
			if (!TargetValueExtra::AreDefaultValues(euler.y, euler.z))
			{
				TargetValueExtra e(getPhysXExporter(), euler.y, euler.z);
			}
		}
	};

	class SpringExtra : public Element
	{
	public:
		SpringExtra(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_SPRING_EXTRA)
		{
			exportLinearExtra(joint);
			exportAngularExtra(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return SpringLinearExtra::HasDefaultValues(joint) &&
				SpringAngularExtra::HasDefaultValues(joint);
		}

	private:
		void exportLinearExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!SpringLinearExtra::HasDefaultValues(joint))
			{
				SpringLinearExtra e(getPhysXExporter(), joint);
			}
		}

		void exportAngularExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!SpringAngularExtra::HasDefaultValues(joint))
			{
				SpringAngularExtra e(getPhysXExporter(), joint);
			}
		}
	};

	class ForceLimit : public Element
	{
	public:
		ForceLimit(PhysXExporter & exporter, double forceLimit)
			: Element(exporter, CSWC::CSW_ELEMENT_FORCE_LIMIT)
		{
			getStreamWriter().appendValues(forceLimit);
		}

		static double DefaultValue()
		{
			return infinite();
		}
	};

	class DriveFlags : public Element
	{
	public:
		DriveFlags(PhysXExporter & exporter, const Flags<PhysXXML::DriveFlags::FlagEnum> & flags)
			: Element(exporter, CSWC::CSW_ELEMENT_DRIVE_FLAGS)
		{
			getStreamWriter().appendValues(PhysXExporter::DriveFlagsToCOLLADA(flags));
		}

		static Flags<PhysXXML::DriveFlags::FlagEnum> DefaultValue()
		{
			return Flags<PhysXXML::DriveFlags::FlagEnum>();
		}
	};

	class LinearX : public Element
	{
	public:
		LinearX(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_X)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
			exportDriveFlags(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveX.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveX.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveX.forceLimit.forceLimit == ForceLimit::DefaultValue() &&
				joint.drive.driveX.flags.flags == DriveFlags::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveX.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveX.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveX.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveX.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveX.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveX.forceLimit.forceLimit);
			}
		}

		void exportDriveFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveX.flags.flags != DriveFlags::DefaultValue())
			{
				DriveFlags e(getPhysXExporter(), joint.drive.driveX.flags.flags);
			}
		}
	};
	
	class LinearY : public Element
	{
	public:
		LinearY(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_Y)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
			exportDriveFlags(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveY.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveY.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveY.forceLimit.forceLimit == ForceLimit::DefaultValue() &&
				joint.drive.driveY.flags.flags == DriveFlags::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveY.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveY.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveY.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveY.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveY.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveY.forceLimit.forceLimit);
			}
		}

		void exportDriveFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveY.flags.flags != DriveFlags::DefaultValue())
			{
				DriveFlags e(getPhysXExporter(), joint.drive.driveY.flags.flags);
			}
		}
	};

	class LinearZ : public Element
	{
	public:
		LinearZ(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_Z)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
			exportDriveFlags(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveZ.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveZ.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveZ.forceLimit.forceLimit == ForceLimit::DefaultValue() &&
				joint.drive.driveZ.flags.flags == DriveFlags::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveZ.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveZ.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveZ.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveZ.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveZ.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveZ.forceLimit.forceLimit);
			}
		}

		void exportDriveFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveZ.flags.flags != DriveFlags::DefaultValue())
			{
				DriveFlags e(getPhysXExporter(), joint.drive.driveZ.flags.flags);
			}
		}
	};

	class Swing : public Element
	{
	public:
		Swing(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_SWING)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
			exportDriveFlags(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveSwing.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveSwing.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveSwing.forceLimit.forceLimit == ForceLimit::DefaultValue() &&
				joint.drive.driveSwing.flags.flags == DriveFlags::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSwing.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveSwing.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSwing.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveSwing.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSwing.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveSwing.forceLimit.forceLimit);
			}
		}

		void exportDriveFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSwing.flags.flags != DriveFlags::DefaultValue())
			{
				DriveFlags e(getPhysXExporter(), joint.drive.driveSwing.flags.flags);
			}
		}
	};

	class Twist : public Element
	{
	public:
		Twist(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_TWIST)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
			exportDriveFlags(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveTwist.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveTwist.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveTwist.forceLimit.forceLimit == ForceLimit::DefaultValue() &&
				joint.drive.driveTwist.flags.flags == DriveFlags::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveTwist.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveTwist.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveTwist.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveTwist.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveTwist.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveTwist.forceLimit.forceLimit);
			}
		}

		void exportDriveFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveTwist.flags.flags != DriveFlags::DefaultValue())
			{
				DriveFlags e(getPhysXExporter(), joint.drive.driveTwist.flags.flags);
			}
		}
	};

	class Slerp : public Element
	{
	public:
		Slerp(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_SLERP)
		{
			exportStiffness(joint);
			exportDamping(joint);
			exportForceLimit(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return joint.drive.driveSlerp.stiffness.stiffness == Stiffness::DefaultValue() &&
				joint.drive.driveSlerp.damping.damping == Damping::DefaultValue() &&
				joint.drive.driveSlerp.forceLimit.forceLimit == ForceLimit::DefaultValue();
		}

	private:
		void exportStiffness(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSlerp.stiffness.stiffness != Stiffness::DefaultValue())
			{
				Stiffness e(getPhysXExporter(), joint.drive.driveSlerp.stiffness.stiffness);
			}
		}

		void exportDamping(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSlerp.damping.damping != Damping::DefaultValue())
			{
				Damping e(getPhysXExporter(), joint.drive.driveSlerp.damping.damping);
			}
		}

		void exportForceLimit(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.drive.driveSlerp.forceLimit.forceLimit != ForceLimit::DefaultValue())
			{
				ForceLimit e(getPhysXExporter(), joint.drive.driveSlerp.forceLimit.forceLimit);
			}
		}
	};

	class LinearVelocity : public Element
	{
	public:
		LinearVelocity(PhysXExporter & exporter, const MVector & linearVelocity)
			: Element(exporter, CSWC::CSW_ELEMENT_LINEAR_VELOCITY)
		{
			getStreamWriter().appendValues(linearVelocity.x, linearVelocity.y, linearVelocity.z);
		}

		static const MVector & DefaultValue()
		{
			return MVector::zero;
		}
	};

	class AngularVelocity : public Element
	{
	public:
		AngularVelocity(PhysXExporter & exporter, const MVector & velocity)
			: Element(exporter, CSWC::CSW_ELEMENT_ANGULAR_VELOCITY)
		{
			getStreamWriter().appendValues(velocity.x, velocity.y, velocity.z);
		}

		static const MVector & DefaultValue()
		{
			return MVector::zero;
		}
	};

	class Drive : public Element
	{
	public:
		Drive(PhysXExporter & exporter, const PhysXXML::PxD6Joint & joint)
			: Element(exporter, CSWC::CSW_ELEMENT_DRIVE)
		{
			exportLinearX(joint);
			exportLinearY(joint);
			exportLinearZ(joint);
			exportSwing(joint);
			exportTwist(joint);
			exportSlerp(joint);
			exportLinearVelocity(joint);
			exportAngularVelocity(joint);
		}

		static bool HasDefaultValues(const PhysXXML::PxD6Joint & joint)
		{
			return LinearX::HasDefaultValues(joint) &&
				LinearY::HasDefaultValues(joint) &&
				LinearZ::HasDefaultValues(joint) &&
				Swing::HasDefaultValues(joint) &&
				Twist::HasDefaultValues(joint) &&
				Slerp::HasDefaultValues(joint) &&
				joint.driveVelocity.linear.linear == LinearVelocity::DefaultValue() &&
				joint.driveVelocity.angular.angular == AngularVelocity::DefaultValue();
		}

	private:
		void exportLinearX(const PhysXXML::PxD6Joint & joint)
		{
			if (!LinearX::HasDefaultValues(joint))
			{
				LinearX e(getPhysXExporter(), joint);
			}
		}

		void exportLinearY(const PhysXXML::PxD6Joint & joint)
		{
			if (!LinearY::HasDefaultValues(joint))
			{
				LinearY e(getPhysXExporter(), joint);
			}
		}

		void exportLinearZ(const PhysXXML::PxD6Joint & joint)
		{
			if (!LinearZ::HasDefaultValues(joint))
			{
				LinearZ e(getPhysXExporter(), joint);
			}
		}

		void exportSwing(const PhysXXML::PxD6Joint & joint)
		{
			if (!Swing::HasDefaultValues(joint))
			{
				Swing e(getPhysXExporter(), joint);
			}
		}

		void exportTwist(const PhysXXML::PxD6Joint & joint)
		{
			if (!Twist::HasDefaultValues(joint))
			{
				Twist e(getPhysXExporter(), joint);
			}
		}

		void exportSlerp(const PhysXXML::PxD6Joint & joint)
		{
			if (!Slerp::HasDefaultValues(joint))
			{
				Slerp e(getPhysXExporter(), joint);
			}
		}

		void exportLinearVelocity(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.driveVelocity.linear.linear != LinearVelocity::DefaultValue())
			{
				LinearVelocity e(getPhysXExporter(), joint.driveVelocity.linear.linear);
			}
		}

		void exportAngularVelocity(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.driveVelocity.angular.angular != AngularVelocity::DefaultValue())
			{
				AngularVelocity e(getPhysXExporter(), joint.driveVelocity.angular.angular);
			}
		}
	};

    class RigidConstraintTechnique : public Element
    {
    public:
        RigidConstraintTechnique(PhysXExporter& exporter, const MObject & rigidConstraint, const String & profile)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, profile);
            if (profile == PhysXExporter::GetProfile()) {
                exporter.exportAttributes(rigidConstraint, GetAttributes());
            }
            else if (profile == PhysXExporter::GetProfileXML()) {
                exporter.exportRigidConstraintPhysXXML(rigidConstraint);
            }
			else if (profile == PROFILE_MAYA) {
				exporter.exportExtraAttributes(rigidConstraint);
			}
			else if (profile == PhysXExporter::GetPhysXProfile()) {
				PhysXXML::PxD6Joint* pxJoint = exporter.findPxD6Joint(rigidConstraint);
				if (pxJoint)
				{
					exportBreakForce(*pxJoint);
					exportBreakTorque(*pxJoint);
					exportConstraintFlags(*pxJoint);
					exportInvMassScale0(*pxJoint);
					exportInvInertiaScale0(*pxJoint);
					exportInvMassScale1(*pxJoint);
					exportInvInertiaScale1(*pxJoint);
					exportProjectionLinearTolerance(*pxJoint);
					exportProjectionAngularTolerance(*pxJoint);
					exportLimitsExtra(*pxJoint);
					exportSpringExtra(*pxJoint);
					exportDrive(*pxJoint);
				}
			}
        }

    private:
        static const std::set<MString, MStringComp>& GetAttributes()
        {
            if (mAttributes.size() == 0)
            {
                // Attributes we want to export in <extra> section
                mAttributes.insert(ATTR_CONSTRAIN);
                mAttributes.insert(ATTR_USE_ACCELERATION);
                mAttributes.insert(ATTR_INTERPENETRATE);
                mAttributes.insert(ATTR_TRANSLATE);
                mAttributes.insert(ATTR_ROTATE);
                mAttributes.insert(ATTR_RADIUS_SCALE);
                mAttributes.insert(ATTR_ORIENTATION_MODE);
                mAttributes.insert(ATTR_MOTION_SWING_Y);
                mAttributes.insert(ATTR_MOTION_SWING_Z);
                mAttributes.insert(ATTR_SWING_1_LIMIT_VALUE);
                mAttributes.insert(ATTR_SWING_1_LIMIT_RESTITUTION);
                mAttributes.insert(ATTR_SWING_1_LIMIT_SPRING);
                mAttributes.insert(ATTR_SWING_1_LIMIT_DAMPING);
                mAttributes.insert(ATTR_SWING_2_LIMIT_VALUE);
                mAttributes.insert(ATTR_SWING_2_LIMIT_RESTITUTION);
                mAttributes.insert(ATTR_SWING_2_LIMIT_SPRING);
                mAttributes.insert(ATTR_SWING_2_LIMIT_DAMPING);
                mAttributes.insert(ATTR_MOTION_TWIST);
                mAttributes.insert(ATTR_TWIST_LOW_LIMIT_VALUE);
                mAttributes.insert(ATTR_TWIST_LOW_LIMIT_RESTITUTION);
                mAttributes.insert(ATTR_TWIST_LOW_LIMIT_SPRING);
                mAttributes.insert(ATTR_TWIST_LOW_LIMIT_DAMPING);
                mAttributes.insert(ATTR_TWIST_HIGH_LIMIT_VALUE);
                mAttributes.insert(ATTR_TWIST_HIGH_LIMIT_RESTITUTION);
                mAttributes.insert(ATTR_TWIST_HIGH_LIMIT_SPRING);
                mAttributes.insert(ATTR_TWIST_HIGH_LIMIT_DAMPING);
                mAttributes.insert(ATTR_MOTION_X);
                mAttributes.insert(ATTR_MOTION_Y);
                mAttributes.insert(ATTR_MOTION_Z);
                mAttributes.insert(ATTR_LINEAR_LIMIT_VALUE);
                mAttributes.insert(ATTR_LINEAR_LIMIT_RESTITUTION);
                mAttributes.insert(ATTR_LINEAR_LIMIT_SPRING);
                mAttributes.insert(ATTR_LINEAR_LIMIT_DAMPING);
                mAttributes.insert(ATTR_DRIVE_DAMPING_X);
                mAttributes.insert(ATTR_DRIVE_DAMPING_Y);
                mAttributes.insert(ATTR_DRIVE_DAMPING_Z);
                mAttributes.insert(ATTR_DRIVE_DAMPING_SWING);
                mAttributes.insert(ATTR_DRIVE_DAMPING_TWIST);
                mAttributes.insert(ATTR_DRIVE_DAMPING_SLERP);
                mAttributes.insert(ATTR_GOAL_SPACE);
                mAttributes.insert(ATTR_GOAL_POSITION);
                mAttributes.insert(ATTR_GOAL_ORIENTATION);
                mAttributes.insert(ATTR_DRIVE_SPRING_X);
                mAttributes.insert(ATTR_DRIVE_SPRING_Y);
                mAttributes.insert(ATTR_DRIVE_SPRING_Z);
                mAttributes.insert(ATTR_DRIVE_SPRING_SWING);
                mAttributes.insert(ATTR_DRIVE_SPRING_TWIST);
                mAttributes.insert(ATTR_DRIVE_SPRING_SLERP);
                mAttributes.insert(ATTR_BREAKABLE_FORCE);
                mAttributes.insert(ATTR_MAX_FORCE);
                mAttributes.insert(ATTR_BREAKABLE_TORQUE);
                mAttributes.insert(ATTR_MAX_TORQUE);
                mAttributes.insert(ATTR_REVERSE);
                mAttributes.insert(ATTR_PROJECTION_MODE);
                mAttributes.insert(ATTR_PROJECTION_DISTANCE);
                mAttributes.insert(ATTR_PROJECTION_ANGLE);
                mAttributes.insert(ATTR_ANGULAR_DRIVE_MODE);
            }
            return mAttributes;
        }

		void exportBreakForce(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.breakForce.force.force != BreakForce::DefaultValue())
			{
				BreakForce e(getPhysXExporter(), joint.breakForce.force.force);
			}
		}

		void exportBreakTorque(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.breakForce.torque.torque != BreakTorque::DefaultValue())
			{
				BreakTorque e(getPhysXExporter(), joint.breakForce.torque.torque);
			}
		}

		void exportConstraintFlags(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.constraintFlags.flags != ConstraintFlags::DefaultValue())
			{
				ConstraintFlags e(getPhysXExporter(), joint.constraintFlags.flags);
			}
		}

		void exportInvMassScale0(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.invMassScale0.invMassScale0 != InvMassScale0::DefaultValue())
			{
				InvMassScale0 e(getPhysXExporter(), joint.invMassScale0.invMassScale0);
			}
		}

		void exportInvInertiaScale0(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.invInertiaScale0.invInertiaScale0 != InvInertiaScale0::DefaultValue())
			{
				InvInertiaScale0 e(getPhysXExporter(), joint.invInertiaScale0.invInertiaScale0);
			}
		}

		void exportInvMassScale1(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.invMassScale1.invMassScale1 != InvMassScale1::DefaultValue())
			{
				InvMassScale1 e(getPhysXExporter(), joint.invMassScale1.invMassScale1);
			}
		}

		void exportInvInertiaScale1(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.invInertiaScale1.invInertiaScale1 != InvInertiaScale1::DefaultValue())
			{
				InvInertiaScale1 e(getPhysXExporter(), joint.invInertiaScale1.invInertiaScale1);
			}
		}

		void exportProjectionLinearTolerance(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.projectionLinearTolerance.projectionLinearTolerance != ProjectionLinearTolerance::DefaultValue())
			{
				ProjectionLinearTolerance e(getPhysXExporter(), joint.projectionLinearTolerance.projectionLinearTolerance);
			}
		}

		void exportProjectionAngularTolerance(const PhysXXML::PxD6Joint & joint)
		{
			if (joint.projectionAngularTolerance.projectionAngularTolerance != ProjectionAngularTolerance::DefaultValue())
			{
				ProjectionAngularTolerance e(getPhysXExporter(), joint.projectionAngularTolerance.projectionAngularTolerance);
			}
		}

		void exportLimitsExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!LimitsExtra::HasDefaultValues(joint))
			{
				LimitsExtra e(getPhysXExporter(), joint);
			}
		}

		void exportSpringExtra(const PhysXXML::PxD6Joint & joint)
		{
			if (!SpringExtra::HasDefaultValues(joint))
			{
				SpringExtra e(getPhysXExporter(), joint);
			}
		}

		void exportDrive(const PhysXXML::PxD6Joint & joint)
		{
			if (!Drive::HasDefaultValues(joint))
			{
				Drive e(getPhysXExporter(), joint);
			}
		}

    private:
        static std::set<MString, MStringComp> mAttributes;
    };
    std::set<MString, MStringComp> RigidConstraintTechnique::mAttributes;

    class RigidConstraintExtra : public Element
    {
    public:
        RigidConstraintExtra(PhysXExporter& exporter, const MObject & rigidConstraint)
            : Element(exporter, CSWC::CSW_ELEMENT_EXTRA)
        {
			if (PhysXExporter::HasExtraAttributes(rigidConstraint)) {
				exportTechnique(rigidConstraint, PROFILE_MAYA);
			}
            exportTechnique(rigidConstraint, PhysXExporter::GetProfile());
            exportTechnique(rigidConstraint, PhysXExporter::GetProfileXML());
			exportTechnique(rigidConstraint, PhysXExporter::GetPhysXProfile());
        }

    private:
        void exportTechnique(const MObject& rigidConstraint, const String& profile)
        {
            RigidConstraintTechnique e(getPhysXExporter(), rigidConstraint, profile);
        }
    };

    class RigidConstraint : public Element
    {
    public:
        RigidConstraint(PhysXExporter& exporter, const MObject & rigidConstraint, const String & sid, const String & name = "")
            : Element(exporter, CSWC::CSW_ELEMENT_RIGID_CONSTRAINT)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            if (name.length() > 0) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
            }

            exportRefAttachment(rigidConstraint);
            exportAttachment(rigidConstraint);
            exportTechniqueCommon(rigidConstraint);
            exportExtra(rigidConstraint);
        }

    private:
        void exportRefAttachment(const MObject & rigidConstraint)
        {
			PhysXXML::PxD6Joint* pxJoint = getPhysXExporter().findPxD6Joint(rigidConstraint);
			if (!pxJoint)
				return;
			PhysXXML::PxRigidDynamic* pxRigidDynamic = getPhysXExporter().findPxRigidDynamic(pxJoint->actors.actor0.actor0);
			PhysXXML::PxRigidStatic* pxRigidStatic = getPhysXExporter().findPxRigidStatic(pxJoint->actors.actor0.actor0);
			
			MObject target;
			if (pxRigidDynamic)
			{
				target = DagHelper::getNode(pxRigidDynamic->name.name.c_str());
			}
			else if (pxRigidStatic)
			{
				target = DagHelper::getNode(pxRigidStatic->name.name.c_str());
			}

			MObject rigidBody = getPhysXExporter().getNodeRigidBody(target);

            URI rigidBodyURI;
            if (rigidBody.isNull())
            {
                rigidBodyURI.set("NULL");
            }
            else
            {
                MDagPath rigidBodyDagPath;
                MDagPath::getAPathTo(rigidBody, rigidBodyDagPath);

                String rigidBodyId = getPhysXExporter().generateColladaId(rigidBodyDagPath);
                rigidBodyURI = getPhysXExporter().getDocumentExporter().getVisualSceneExporter()->getSceneElementURI(rigidBodyDagPath, rigidBodyId);
            }

            RefAttachment e(getPhysXExporter(), rigidConstraint, rigidBodyURI);
        }

        void exportAttachment(const MObject & rigidConstraint)
        {
			PhysXXML::PxD6Joint* pxJoint = getPhysXExporter().findPxD6Joint(rigidConstraint);
			if (!pxJoint)
				return;
			PhysXXML::PxRigidDynamic* pxRigidDynamic = getPhysXExporter().findPxRigidDynamic(pxJoint->actors.actor1.actor1);
			PhysXXML::PxRigidStatic* pxRigidStatic = getPhysXExporter().findPxRigidStatic(pxJoint->actors.actor1.actor1);

			MObject target;
			if (pxRigidDynamic)
			{
				target = DagHelper::getNode(pxRigidDynamic->name.name.c_str());
			}
			else if (pxRigidStatic)
			{
				target = DagHelper::getNode(pxRigidStatic->name.name.c_str());
			}

			MObject rigidBody = getPhysXExporter().getNodeRigidBody(target);

            MDagPath rigidBodyDagPath;
            MDagPath::getAPathTo(rigidBody, rigidBodyDagPath);

            String rigidBodyId = getPhysXExporter().generateColladaId(rigidBodyDagPath);
            URI rigidBodyURI = getPhysXExporter().getDocumentExporter().getVisualSceneExporter()->getSceneElementURI(rigidBodyDagPath, rigidBodyId);

            Attachment e(getPhysXExporter(), rigidConstraint, rigidBodyURI);
        }

        void exportTechniqueCommon(const MObject & rigidConstraint)
        {
            RigidConstraintTechniqueCommon e(getPhysXExporter(), rigidConstraint);
        }

        void exportExtra(const MObject & rigidConstraint)
        {
            RigidConstraintExtra e(getPhysXExporter(), rigidConstraint);
        }
    };

    class Velocity : public Element
    {
    public:
        Velocity(PhysXExporter & exporter, const MVector & velocity)
            : Element(exporter, CSWC::CSW_ELEMENT_VELOCITY)
        {
            getStreamWriter().appendValues(velocity.x, velocity.y, velocity.z);
        }
    };

    class InstanceRigidBodyTechniqueCommon : public Element
    {
    public:
        InstanceRigidBodyTechniqueCommon(PhysXExporter& exporter, const MVector & angularVelocity, const MVector& velocity)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE_COMMON)
        {
            exportAngularVelocity(angularVelocity);
            exportVelocity(velocity);

            // Don't export other elements, use rigid_body's ones.
        }

    private:
        void exportAngularVelocity(const MVector& angularVelocity)
        {
            if (angularVelocity != MVector::zero)
            {
                AngularVelocity e(getPhysXExporter(), angularVelocity);
            }
        }

        void exportVelocity(const MVector& velocity)
        {
            if (velocity != MVector::zero)
            {
                Velocity e(getPhysXExporter(), velocity);
            }
        }
    };

    class InstanceRigidBody : public Element
    {
    public:
        InstanceRigidBody(PhysXExporter& exporter, const MObject & rigidBody, const String& instanceRigidBodySID, const String & rigidBodySID, const URI & targetURI)
            : Element(exporter, CSWC::CSW_ELEMENT_INSTANCE_RIGID_BODY)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, instanceRigidBodySID);
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_BODY, rigidBodySID);
            getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_TARGET, targetURI);

            MVector initialSpin = MVector::zero;
            DagHelper::getPlugValue(rigidBody, ATTR_INITIAL_SPIN, initialSpin);
            initialSpin.x = COLLADABU::Math::Utils::radToDeg(initialSpin.x);
            initialSpin.y = COLLADABU::Math::Utils::radToDeg(initialSpin.y);
            initialSpin.z = COLLADABU::Math::Utils::radToDeg(initialSpin.z);

            MVector initialVelocity = MVector::zero;
            DagHelper::getPlugValue(rigidBody, ATTR_INITIAL_VELOCITY, initialVelocity);
            initialVelocity.x = MDistance::internalToUI(initialVelocity.x);
            initialVelocity.y = MDistance::internalToUI(initialVelocity.y);
            initialVelocity.z = MDistance::internalToUI(initialVelocity.z);

            exportTechniqueCommon(initialSpin, initialVelocity);
        }

    private:
        void exportTechniqueCommon(const MVector& angularVelocity, const MVector& velocity)
        {
            if (angularVelocity != MVector::zero ||
                velocity != MVector::zero)
            {
                InstanceRigidBodyTechniqueCommon e(getPhysXExporter(), angularVelocity, velocity);
            }
        }
    };

    class InstanceRigidConstraint : public Element
    {
    public:
        InstanceRigidConstraint(PhysXExporter & exporter, const String& instanceConstraintSid, const String & constraintSid)
            : Element(exporter, CSWC::CSW_ELEMENT_INSTANCE_RIGID_CONSTRAINT)
        {
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, instanceConstraintSid);
            getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_CONSTRAINT, constraintSid);
        }
    };

    class InstancePhysicsModel : public Element
    {
    public:
        InstancePhysicsModel(PhysXExporter& exporter, const String& sid, const URI& uri)
            : Element(exporter, CSWC::CSW_ELEMENT_INSTANCE_PHYSICS_MODEL)
        {
            if (!sid.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            }
            getStreamWriter().appendURIAttribute(CSWC::CSW_ATTRIBUTE_URL, uri);

            exportInstanceRigidBodies();
            exportInstanceRigidConstraints();
        }

    private:
        class RigidBodyParser
        {
        public:
            RigidBodyParser(PhysXExporter & exporter)
            : mPhysXExporter(exporter)
            , mInstanceRigidBodyIndex(0)
            {}
            
            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_BODY &&
                    element.getIsLocal())
                {
                    const MObject & rigidBody = element.getNode();
                    
                    MObject target;
                    mPhysXExporter.getRigidBodyTarget(rigidBody, target);
                    MDagPath targetDagPath;
                    MDagPath::getAPathTo(target, targetDagPath);
                    SceneElement* targetElement = mPhysXExporter.getDocumentExporter().getSceneGraph()->findElement(targetDagPath);
                    if (targetElement)
                    {
                        String rigidBodySid = mPhysXExporter.generateColladaId(element.getPath());
                        std::stringstream s;
                        s << "_" << mInstanceRigidBodyIndex++;
                        String instanceRigidBodySid = rigidBodySid + s.str();
                        URI targetURI = mPhysXExporter.getDocumentExporter().getVisualSceneExporter()->getSceneElementURI(targetElement);
                        InstanceRigidBody e(mPhysXExporter, rigidBody, instanceRigidBodySid, rigidBodySid, targetURI);
                    }
                }
                return true;
            }
            
        private:
            PhysXExporter & mPhysXExporter;
            int mInstanceRigidBodyIndex;
        };
        
        void exportInstanceRigidBodies()
        {
            PhysXExporter& exporter = getPhysXExporter();

            // Export ground plane if enabled
            MObject rigidSolver;
            if (exporter.getRigidSolver(rigidSolver))
            {
                String name = GROUND_PLANE_NAME;
                String sid = name;

                bool useGroundPlane = false;
                DagHelper::getPlugValue(rigidSolver, ATTR_USE_GROUND_PLANE, useGroundPlane);
                PhysXXML::PxRigidStatic* groundPlane = exporter.findPxRigidStatic(name);
                if (useGroundPlane && groundPlane)
                {
                    // Can't use Element class since ground plane has no associated MObject.
                    // So export everything manually...

                    StreamWriter& sw = getStreamWriter();
                    sw.openElement(CSWC::CSW_ELEMENT_INSTANCE_RIGID_BODY);
                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_SID, name);
                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_BODY, name);
                    // No target
                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_TARGET, "#");
                    sw.closeElement();
                }
            }

            RigidBodyParser rigidBodyParser(exporter);
            exporter.parseSceneElements(rigidBodyParser);
        }

        class RigidConstraintParser
        {
        public:
            RigidConstraintParser(PhysXExporter & exporter)
            : mPhysXExporter(exporter)
            , mInstanceRigidConstraintIndex(0)
            {}
            
            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_CONSTRAINT &&
                    element.getIsLocal())
                {
                    const MObject & rigidConstraint = element.getNode();
                    
                    String rigidConstraintSid = mPhysXExporter.generateColladaId(element.getPath());
                    std::stringstream s;
                    s << "_" << mInstanceRigidConstraintIndex++;
                    String instanceRigidConstraintSid = rigidConstraintSid + s.str();
                    InstanceRigidConstraint e(mPhysXExporter, instanceRigidConstraintSid, rigidConstraintSid);
                }
                return true;
            }
            
        private:
            PhysXExporter & mPhysXExporter;
            int mInstanceRigidConstraintIndex;
        };
        
        void exportInstanceRigidConstraints()
        {
            RigidConstraintParser rigidConstraintParser(getPhysXExporter());
            getPhysXExporter().parseSceneElements(rigidConstraintParser);
        }
    };

    class PhysicsModel : public Element
    {
    public:
        PhysicsModel(PhysXExporter& exporter, const String & id)
            : Element(exporter, CSWC::CSW_ELEMENT_PHYSICS_MODEL)
        {
            if (!id.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_ID, id);
            }
            exportRigidBodies();
            exportRigidConstraints();
            //exportInstancePhysicsModels();
        }

    private:
        class RigidBodyParser
        {
        public:
            RigidBodyParser(PhysXExporter & exporter)
            : mPhysXExporter(exporter)
            {}
            
            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_BODY &&
                    element.getIsLocal())
                {
                    String rigidBodySid = mPhysXExporter.generateColladaId(element.getPath());
                    String rigidBodyName = mPhysXExporter.generateColladaName(element.getPath());
                    
                    const MObject & rigidBody = element.getNode();
                    
                    RigidBody e(mPhysXExporter, rigidBody, rigidBodySid, rigidBodyName);
                }
                return true;
            }
            
        private:
            PhysXExporter & mPhysXExporter;
        };
        
        void exportRigidBodies()
        {
            PhysXExporter& exporter = getPhysXExporter();

            // Export ground plane if enabled
            MObject rigidSolver;
            if (exporter.getRigidSolver(rigidSolver))
            {
                String name = GROUND_PLANE_NAME;
                String sid = name;

                bool useGroundPlane = false;
                DagHelper::getPlugValue(rigidSolver, ATTR_USE_GROUND_PLANE, useGroundPlane);
                PhysXXML::PxRigidStatic* groundPlane = exporter.findPxRigidStatic(name);
                if (useGroundPlane && groundPlane)
                {
                    // Can't use Element class since ground plane has no associated MObject.
                    // So export everything manually...

                    StreamWriter& sw = getStreamWriter();
                    sw.openElement(CSWC::CSW_ELEMENT_RIGID_BODY);
                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
                    {
                        sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE_COMMON);
                        {
                            sw.openElement(CSWC::CSW_ELEMENT_RIGID_BODY_DYNAMIC);
                            sw.appendValues(false);
                            sw.closeElement();

                            PhysXXML::PxMaterial* mat = exporter.findPxMaterial(groundPlane->shapes.shapes[0].materials.materialRef.materialRef);
                            if (mat)
                            {
                                sw.openElement(CSWC::CSW_ELEMENT_PHYSICS_MATERIAL);
                                sw.appendAttribute(CSWC::CSW_ATTRIBUTE_ID, name + "_material");
                                {
                                    sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE_COMMON);
                                    {
                                        sw.openElement(CSWC::CSW_ELEMENT_RESTITUTION);
                                        sw.appendValues(mat->restitution.restitution);
                                        sw.closeElement();

                                        sw.openElement(CSWC::CSW_ELEMENT_DYNAMIC_FRICTION);
                                        sw.appendValues(mat->dynamicFriction.dynamicFriction);
                                        sw.closeElement();

                                        sw.openElement(CSWC::CSW_ELEMENT_STATIC_FRICTION);
                                        sw.appendValues(mat->staticFriction.staticFriction);
                                        sw.closeElement();
                                    }
                                    sw.closeElement();

                                    sw.openElement(CSWC::CSW_ELEMENT_EXTRA);
                                    {
										sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE);
										sw.appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, PhysXExporter::GetPhysXProfile());
										{
											sw.openElement(CSWC::CSW_ELEMENT_FRICTION_COMBINE_MODE);
											sw.appendValues(PhysXExporter::CombineModeToCOLLADA(mat->frictionCombineMode.frictionCombineMode));
											sw.closeElement();

											sw.openElement(CSWC::CSW_ELEMENT_RESTITUTION_COMBINE_MODE);
											sw.appendValues(PhysXExporter::CombineModeToCOLLADA(mat->restitutionCombineMode.restitutionCombineMode));
											sw.closeElement();
										}
										sw.closeElement();

                                        sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE);
                                        sw.appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, PhysXExporter::GetProfileXML());
                                        {
                                            mat->exportElement(sw);
                                        }
                                        sw.closeElement();
                                    }
                                    sw.closeElement();
                                }
                                sw.closeElement();
                            }

                            sw.openElement(CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE);
                            {
                                PhysXXML::PxShape& shape = groundPlane->shapes.shapes[0];

                                sw.openElement(CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_PLANE);
                                {
                                    double groundPlanePosition = 0.0f;
                                    DagHelper::getPlugValue(rigidSolver, ATTR_GROUND_PLANE_POSITION, groundPlanePosition);
                                    groundPlanePosition = MDistance::internalToUI(groundPlanePosition);

                                    // Plane equation
                                    double a = 0.0f;
                                    double b = 1.0f;
                                    double c = 0.0f;
                                    double d = -groundPlanePosition;

                                    sw.openElement(CSWC::CSW_ELEMENT_RIGID_BODY_SHAPE_PLANE_EQUATION);
                                    sw.appendValues(a, b, c, d);
                                    sw.closeElement();
                                    
                                }
                                sw.closeElement();

                                sw.openElement(CSWC::CSW_ELEMENT_EXTRA);
                                {
                                    sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE);
                                    sw.appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, PhysXExporter::GetProfileXML());
                                    {
                                        shape.exportElement(sw);
                                    }
                                    sw.closeElement();

									sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE);
									sw.appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, PhysXExporter::GetPhysXProfile());
									{
										sw.openElement(CSWC::CSW_ELEMENT_SIMULATION_FILTER_DATA);
										sw.appendValues(shape.simulationFilterData.filter0, shape.simulationFilterData.filter1, shape.simulationFilterData.filter2, shape.simulationFilterData.filter3);
										sw.closeElement();

										sw.openElement(CSWC::CSW_ELEMENT_QUERY_FILTER_DATA);
										sw.appendValues(shape.queryFilterData.filter0, shape.queryFilterData.filter1, shape.queryFilterData.filter2, shape.queryFilterData.filter3);
										sw.closeElement();

										sw.openElement(CSWC::CSW_ELEMENT_CONTACT_OFFSET);
										sw.appendValues(shape.contactOffset.contactOffset);
										sw.closeElement();

										sw.openElement(CSWC::CSW_ELEMENT_REST_OFFSET);
										sw.appendValues(shape.restOffset.restOffset);
										sw.closeElement();

										sw.openElement(CSWC::CSW_ELEMENT_SHAPE_FLAGS);
										sw.appendValues(PhysXExporter::ShapeFlagsToCOLLADA(shape.flags.flags));
										sw.closeElement();

										sw.openElement(CSWC::CSW_ELEMENT_NAME);
										sw.appendValues(shape.name.name);
										sw.closeElement();
									}
									sw.closeElement();
                                }
                                sw.closeElement();
                            }
                            sw.closeElement();
                        }
                        sw.closeElement();

                        sw.openElement(CSWC::CSW_ELEMENT_EXTRA);
                        {
                            sw.openElement(CSWC::CSW_ELEMENT_TECHNIQUE);
                            sw.appendAttribute(CSWC::CSW_ATTRIBUTE_PROFILE, PhysXExporter::GetProfileXML());
                            {
                                groundPlane->exportElement(sw);
                            }
                            sw.closeElement();
                        }
                        sw.closeElement();
                    }
                    sw.closeElement();
                }
            }

            // Export the other rigid bodies
            RigidBodyParser rigidBodyParser(exporter);
            exporter.parseSceneElements(rigidBodyParser);
        }

        class RigidConstraintParser
        {
        public:
            RigidConstraintParser(PhysXExporter & exporter)
            : mPhysXExporter(exporter)
            {}
            
            bool operator()(SceneElement & element)
            {
                if (element.getType() == SceneElement::PHYSX_RIGID_CONSTRAINT &&
                    element.getIsLocal())
                {
                    String rigidConstraintSid = mPhysXExporter.generateColladaId(element.getPath());
                    String rigidConstraintName = mPhysXExporter.generateColladaName(element.getPath());
                    
                    const MObject & rigidConstraint = element.getNode();
                    
                    RigidConstraint e(mPhysXExporter, rigidConstraint, rigidConstraintSid, rigidConstraintName);
                }
                return true;
            }
            
        private:
            PhysXExporter & mPhysXExporter;
        };
        
        void exportRigidConstraints()
        {
            RigidConstraintParser rigidConstraintParser(getPhysXExporter());
            getPhysXExporter().parseSceneElements(rigidConstraintParser);
        }
    };

    class LibraryPhysicsModels : public Element
    {
    public:
        LibraryPhysicsModels(PhysXExporter& exporter)
            : Element(exporter, CSWC::CSW_ELEMENT_LIBRARY_PHYSICS_MODELS)
        {
            PhysicsModel e(exporter, PhysXExporter::GetDefaultPhysicsModelId());
        }
    };

    class Gravity : public Element
    {
    public:
        Gravity(PhysXExporter & exporter, const MVector & gravity)
            : Element(exporter, CSWC::CSW_ELEMENT_GRAVITY)
        {
            getStreamWriter().appendValues(gravity.x, gravity.y, gravity.z);
        }
    };

    class PhysicsSceneTechniqueCommon : public Element
    {
    public:
        PhysicsSceneTechniqueCommon(PhysXExporter & exporter)
            : Element(exporter, CSWC::CSW_ELEMENT_TECHNIQUE_COMMON)
        {
            MObject rigidSolver;
            if (exporter.getRigidSolver(rigidSolver))
            {
                exportGravity(rigidSolver);
                exportTimeStep(rigidSolver);
            }
        }

    private:
        void exportGravity(const MObject & rigidSolver)
        {
            bool gravityEnabled = false;
            DagHelper::getPlugValue(rigidSolver, ATTR_GRAVITY, gravityEnabled);
            if (gravityEnabled)
            {
                double gravityMagnitude = 9.18;
                DagHelper::getPlugValue(rigidSolver, ATTR_GRAVITY_MAGNITUDE, gravityMagnitude);

                MVector gravityDirection(0.0, -1.0, 0.0);
                DagHelper::getPlugValue(rigidSolver, ATTR_GRAVITY_DIRECTION, gravityDirection);
                
                gravityDirection.x = MDistance::internalToUI(gravityDirection.x);
                gravityDirection.y = MDistance::internalToUI(gravityDirection.y);
                gravityDirection.z = MDistance::internalToUI(gravityDirection.z);

                Gravity e(getPhysXExporter(), gravityDirection * gravityMagnitude);
            }
        }

        void exportTimeStep(const MObject & rigidSolver)
        {
            
        }
    };

    class PhysicsScene : public Element
    {
    public:
        PhysicsScene(PhysXExporter& exporter, const String & id)
            : Element(exporter, CSWC::CSW_ELEMENT_PHYSICS_SCENE)
        {
            if (!id.empty()) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_ID, id);
            }
            exportInstancePhysicsModels();
            exportTechniqueCommon();
        }

    private:
        void exportInstancePhysicsModels()
        {
            // Local physics model
            if (getPhysXExporter().sceneHas(SceneElement::PHYSX_RIGID_BODY, PhysXExporter::Local) ||
                getPhysXExporter().sceneHas(SceneElement::PHYSX_RIGID_CONSTRAINT, PhysXExporter::Local))
            {
                String physicsModelId = PhysXExporter::GetDefaultPhysicsModelId();
                
                URI uri;
                uri.set("");
                uri.setFragment(physicsModelId);

                InstancePhysicsModel e(getPhysXExporter(), PhysXExporter::GetDefaultInstancePhysicsModelSid(), uri);
            }
        }

        void exportTechniqueCommon()
        {
            PhysicsSceneTechniqueCommon e(getPhysXExporter());
        }
    };

    class LibraryPhysicsScenes : public Element
    {
    public:
        LibraryPhysicsScenes(PhysXExporter& exporter)
            : Element(exporter, CSWC::CSW_ELEMENT_LIBRARY_PHYSICS_SCENES)
        {
            PhysicsScene e(exporter, PhysXExporter::GetDefaultPhysicsSceneId());
        }
    };

    class Translate : public Element
    {
    public:
        Translate(PhysXExporter& exporter, const MVector & translation, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_TRANSLATE)
        {
            if (sid.length() > 0)
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            getStreamWriter().appendValues(translation.x, translation.y, translation.z);
        }
    };

    class Rotate : public Element
    {
    public:
        Rotate(PhysXExporter& exporter, const MVector & axis, double angle, const String & sid = "")
            : Element(exporter, CSWC::CSW_ELEMENT_ROTATE)
        {
            if (sid.length() > 0) {
                getStreamWriter().appendAttribute(CSWC::CSW_ATTRIBUTE_SID, sid);
            }
            getStreamWriter().appendValues(axis.x, axis.y, axis.z, angle);
        }
    };

    String PhysXExporter::mDefaultPhysicsModelId = "collada_physics_model";
    String PhysXExporter::mDefaultPhysicsSceneId = "collada_physics_scene";
    String PhysXExporter::mDefaultInstancePhysicsModelSid = "instancePhysicsModel";
    String PhysXExporter::mProfile = "OpenCOLLADAMayaPhysX";
    String PhysXExporter::mProfileXML = "OpenCOLLADAMayaPhysXXML";
	String PhysXExporter::mPhysXProfile = "PhysX_3.x";
	std::map<PhysXXML::CombineMode::FlagEnum, String> PhysXExporter::mCombineModeMap = PhysXExporter::InitializeCombineModeMap();
	std::map<PhysXXML::ShapeFlags::FlagEnum, String> PhysXExporter::mShapeFlagMap = PhysXExporter::InitializeShapeFlagMap();
	std::map<PhysXXML::ActorFlags::FlagEnum, String> PhysXExporter::mActorFlagMap = PhysXExporter::InitializeActorFlagMap();
	std::map<PhysXXML::RigidBodyFlags::FlagEnum, String> PhysXExporter::mRigidBodyFlagMap = PhysXExporter::InitializeRigidBodyFlagMap();
	std::map<PhysXXML::ConstraintFlags::FlagEnum, String> PhysXExporter::mConstraintFlagMap = PhysXExporter::InitializeConstraintFlagMap();
	std::map<PhysXXML::DriveFlags::FlagEnum, String> PhysXExporter::mDriveFlagMap = PhysXExporter::InitializeDriveFlagMap();

	std::map<PhysXXML::CombineMode::FlagEnum, String> PhysXExporter::InitializeCombineModeMap()
	{
		std::map<PhysXXML::CombineMode::FlagEnum, String> m;
		m[PhysXXML::CombineMode::FlagEnum::Average] = "AVERAGE";
		m[PhysXXML::CombineMode::FlagEnum::Min] = "MIN";
		m[PhysXXML::CombineMode::FlagEnum::Multiply] = "MULTIPLY";
		m[PhysXXML::CombineMode::FlagEnum::Max] = "MAX";
		return m;
	}

	std::map<PhysXXML::ShapeFlags::FlagEnum, String> PhysXExporter::InitializeShapeFlagMap()
	{
		std::map<PhysXXML::ShapeFlags::FlagEnum, String> m;
		m[PhysXXML::ShapeFlags::FlagEnum::SimulationShape] = "SIMULATION_SHAPE";
		m[PhysXXML::ShapeFlags::FlagEnum::SceneQueryShape] = "SCENE_QUERY_SHAPE";
		m[PhysXXML::ShapeFlags::FlagEnum::TriggerShape] = "TRIGGER_SHAPE";
		m[PhysXXML::ShapeFlags::FlagEnum::Visualization] = "VISUALIZATION";
		m[PhysXXML::ShapeFlags::FlagEnum::ParticleDrain] = "PARTICLE_DRAIN";
		return m;
	}

	std::map<PhysXXML::ActorFlags::FlagEnum, String> PhysXExporter::InitializeActorFlagMap()
	{
		std::map<PhysXXML::ActorFlags::FlagEnum, String> m;
		m[PhysXXML::ActorFlags::FlagEnum::Visualization] = "VISUALIZATION";
		m[PhysXXML::ActorFlags::FlagEnum::DisableGravity] = "DISABLE_GRAVITY";
		m[PhysXXML::ActorFlags::FlagEnum::SendSleepNotifies] = "SEND_SLEEP_NOTIFIES";
		m[PhysXXML::ActorFlags::FlagEnum::DisableSimulation] = "DISABLE_SIMULATION";
		return m;
	}

	std::map<PhysXXML::RigidBodyFlags::FlagEnum, String> PhysXExporter::InitializeRigidBodyFlagMap()
	{
		std::map<PhysXXML::RigidBodyFlags::FlagEnum, String> m;
		m[PhysXXML::RigidBodyFlags::FlagEnum::Kinematic] = "KINEMATIC";
		m[PhysXXML::RigidBodyFlags::FlagEnum::UseKinematicTargetForSceneQueries] = "USE_KINEMATIC_TARGET_FOR_SCENE_QUERIES";
		m[PhysXXML::RigidBodyFlags::FlagEnum::EnabledCCD] = "ENABLE_CCD";
		m[PhysXXML::RigidBodyFlags::FlagEnum::EnabledCCDFriction] = "ENABLE_CCD_FRICTION";
		return m;
	}

	std::map<PhysXXML::ConstraintFlags::FlagEnum, String> PhysXExporter::InitializeConstraintFlagMap()
	{
		std::map<PhysXXML::ConstraintFlags::FlagEnum, String> m;
		m[PhysXXML::ConstraintFlags::FlagEnum::Broken] = "BROKEN";
		m[PhysXXML::ConstraintFlags::FlagEnum::ProjectToActor0] = "PROJECT_TO_ACTOR0";
		m[PhysXXML::ConstraintFlags::FlagEnum::ProjectToActor1] = "PROJECT_TO_ACTOR1";
		m[PhysXXML::ConstraintFlags::FlagEnum::Projection] = "PROJECTION";
		m[PhysXXML::ConstraintFlags::FlagEnum::CollisionEnabled] = "COLLISION_ENABLED";
		m[PhysXXML::ConstraintFlags::FlagEnum::Reporting] = "REPORTING";
		m[PhysXXML::ConstraintFlags::FlagEnum::Visualization] = "VISUALIZATION";
		m[PhysXXML::ConstraintFlags::FlagEnum::DriveLimitsAreForces] = "DRIVE_LIMITS_ARE_FORCES";
		m[PhysXXML::ConstraintFlags::FlagEnum::ImprovedSlerp] = "IMPROVED_SLERP";
		return m;
	}

	std::map<PhysXXML::DriveFlags::FlagEnum, String> PhysXExporter::InitializeDriveFlagMap()
	{
		std::map<PhysXXML::DriveFlags::FlagEnum, String> m;
		m[PhysXXML::DriveFlags::FlagEnum::Acceleration] = "ACCELERATION";
		return m;
	}

    PhysXExporter::PhysXExporter(StreamWriter& streamWriter, DocumentExporter& documentExporter)
        : mStreamWriter(streamWriter)
        , mDocumentExporter(documentExporter)
    {}
    
    class RigidBodyParser
    {
    public:
        RigidBodyParser(const SceneElement & meshElement)
        : mMeshElement(meshElement)
        , mNeedsConvexHullOfMeshElement(false)
        {}
        
        bool operator()(SceneElement & element)
        {
            if (element.getType() == SceneElement::PHYSX_RIGID_BODY)
            {
                std::vector<MObject> shapes;
                PhysXExporter::GetRigidBodyShapes(element.getNode(), shapes);
                
                for (size_t i = 0; i < shapes.size(); ++i)
                {
                    const MObject & shape = shapes[i];
                    int dummy = 0;
                    MString shapeType;
                    DagHelper::getPlugValue(shape, ATTR_SHAPE_TYPE, dummy, shapeType);
                    if (shapeType == SHAPE_TYPE_CONVEX_HULL)
                    {
                        MObject connectedMesh;
                        PhysXShape::GetConnectedInMesh(shape, connectedMesh);
                        
                        if (mMeshElement.getNode() == connectedMesh ||
                            mMeshElement.getNode() == shape)
                        {
                            mNeedsConvexHullOfMeshElement = true;
                            mShape = shape;
                            return false;
                        }
                    }
                }
            }
            return true;
        }
        
        bool needsConvexHullOfMeshElement() const
        {
            return mNeedsConvexHullOfMeshElement;
        }

        const MObject& getShape() const
        {
            return mShape;
        }
        
    private:
        const SceneElement & mMeshElement;
        bool mNeedsConvexHullOfMeshElement;
        MObject mShape;
    };

    bool PhysXExporter::needsConvexHullOf(const SceneElement & meshElement, MObject& shape)
    {
        if (!ExportOptions::exportPhysics())
            return false;
        
        RigidBodyParser parser(meshElement);
        parseSceneElements(parser);
        shape = parser.getShape();
        return parser.needsConvexHullOfMeshElement();
    }

    bool ExtractPhysXPluginVersionNumbers(
        int & major,
        int & minor,
        int & a,
        int & b)
    {
        String version = PhysXExporter::GetInstalledPhysXPluginVersion().asChar();

        size_t p1 = version.find('(');
        if (p1 == String::npos) return false;
        ++p1;
        if (p1 >= version.length()) return false;

        size_t p2 = version.find(')', p1);
        if (p2 == String::npos) return false;

        MString mnumbers = version.substr(p1, p2 - p1).c_str();
        MStringArray numbers;
        mnumbers.split('.', numbers);
        if (numbers.length() != 4) return false;

        std::stringstream s;
        s << numbers[0] << ' ' << numbers[1] << ' ' << numbers[2] << ' ' << numbers[3];
        s >> major >> minor >> a >> b;

        return true;
    }

    bool PhysXExporter::CheckPhysXPluginVersion()
    {
        MObject pluginObject = MFnPlugin::findPlugin("physx");

        if (pluginObject.isNull()) {
            return false;
        }

        MFnPlugin fnPlugin(pluginObject);

        MStatus status;
        MString mversion = fnPlugin.version(&status);
        if (!status) return false;

        int requ_major = 0;
        int requ_minor = 0;
        int requ_a = 0;
        int requ_b = 0;

        int curr_major = 0;
        int curr_minor = 0;
        int curr_a = 0;
        int curr_b = 0;

        if (!ExtractPhysXPluginVersionNumbers(requ_major, requ_minor, requ_a, requ_b)) {
            return false;
        }

        if (!ExtractPhysXPluginVersionNumbers(curr_major, curr_minor, curr_a, curr_b)) {
            return false;
        }

        if (curr_major < requ_major ||
            curr_minor < requ_minor ||
            curr_a < requ_a ||
            curr_b < requ_b) {
            return false;
        }

        return true;
    }

    MString PhysXExporter::GetRequiredPhysXPluginVersion()
    {
        return MString("PhysxForMaya (3.3.10708.21402) , compiled 7/8/2015 9:40:44 PM");
    }

    MString PhysXExporter::GetInstalledPhysXPluginVersion()
    {
        static MString na = "N/A";

        MObject pluginObject = MFnPlugin::findPlugin("physx");
        if (pluginObject.isNull()) {
            return na;
        }

        MFnPlugin fnPlugin(pluginObject);

        MStatus status;
        MString version = fnPlugin.version(&status);
        if (!status) return na;

        return version;
    }

    class AutoDeleteFile
    {
    public:
        AutoDeleteFile(const String& filePath)
            : mFilePath(filePath)
        {}

        ~AutoDeleteFile()
        {
            if (!mFilePath.empty()) {
                COLLADABU::Utils::deleteFile(mFilePath);
            }
        }

    private:
        String mFilePath;
    };

    class AutoXmlFreeDoc
    {
    public:
        AutoXmlFreeDoc(xmlDocPtr xml)
            : mXml(xml)
        {}

        ~AutoXmlFreeDoc()
        {
            if (mXml) {
                xmlFreeDoc(mXml);
            }
        }

    private:
        xmlDocPtr mXml;
    };

    class AutoRestorePhysXExportOptions
    {
    public:
        AutoRestorePhysXExportOptions()
            : mError(false)
        {
            mOptions.push_back(Option("apexClothingExport_APBs", Integer));
            mOptions.push_back(Option("validatePhysXSceneBeforeExport", Integer));
            mOptions.push_back(Option("PhysXExport_useFolderName", Integer));
            mOptions.push_back(Option("apexExport_RemoveNamespaceForJoint", Integer));
            mOptions.push_back(Option("apexClothingExport_VisibleOnly", Integer));
            mOptions.push_back(Option("PhysXExport_UseJointLongName", Integer));
            mOptions.push_back(Option("PhysXExport_exportPxProjFile", Integer));
            mOptions.push_back(Option("PhysXExport_exportPhysX", Integer));
            mOptions.push_back(Option("PhysXExport_outputUnit", String));
            mOptions.push_back(Option("PhysXExport_customScaling", Integer));
            mOptions.push_back(Option("PhysXExport_outputScale", Double));

            for (size_t i = 0; i < mOptions.size(); ++i) {
                switch (mOptions[i].type)
                {
                case Integer:
                {
                    int oldValue = 0;
                    MStatus status = MGlobal::executeCommand("optionVar -q \"" + mOptions[i].name + "\"", oldValue);
                    if (!status) mError |= true;
                    MString oldValueStr("");
                    oldValueStr += oldValue;
                    mOldValues.push_back(oldValueStr);
                    break;
                }
                case Double:
                {
                    double oldValue = 0.0;
                    MStatus status = MGlobal::executeCommand("optionVar -q \"" + mOptions[i].name + "\"", oldValue);
                    if (!status) mError |= true;
                    MString oldValueStr("");
                    oldValueStr += oldValue;
                    mOldValues.push_back(oldValueStr);
                    break;
                }
                case String:
                {
                    MString oldValue;
                    MStatus status = MGlobal::executeCommand("optionVar -q \"" + mOptions[i].name + "\"", oldValue);
                    if (!status) mError |= true;
                    mOldValues.push_back(oldValue);
                    break;
                }
                }
            }
        }

        ~AutoRestorePhysXExportOptions()
        {
            for (size_t i = 0; i < mOptions.size(); ++i) {
                MString command = "optionVar -";
                switch (mOptions[i].type)
                {
                case Integer:   command += "i"; break;
                case Double:     command += "f"; break;
                case String:    command += "s"; break;
                }
                MGlobal::executeCommand(command + "v \"" + mOptions[i].name + "\" " + mOldValues[i]);
            }
        }

        bool error() const
        {
            return mError;
        }

    private:
        enum OptionType
        {
            Integer,
            Double,
            String
        };

        struct Option
        {
            Option(const MString& name_, OptionType type_)
                : name(name_)
                , type(type_)
            {}

            MString name;
            OptionType type;
        };
        std::vector<Option> mOptions;
        std::vector<MString> mOldValues;
        bool mError;
    };

    bool PhysXExporter::generatePhysXXML()
    {
        MStatus status;

        // Backup export options
        AutoRestorePhysXExportOptions autoRestorePhysXExportOptions;

        // PhysX internal data is in centimeters and we need to export to UI unit.
        MDistance unitDistance = MDistance(1.0, MDistance::uiUnit());
        double asCentimeters = unitDistance.asCentimeters();
        double centimetersToUIUnit = 1.0 / asCentimeters;
        MString centimetersToUIUnitStr = "";
        centimetersToUIUnitStr += centimetersToUIUnit;

        // Set export options
        status = MGlobal::executeCommand("optionVar -iv \"apexClothingExport_APBs\" 2");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"validatePhysXSceneBeforeExport\" 0");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"PhysXExport_useFolderName\" 0");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"apexExport_RemoveNamespaceForJoint\" 0");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"apexClothingExport_VisibleOnly\" 0");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"PhysXExport_UseJointLongName\" 1");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"PhysXExport_exportPxProjFile\" 0");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"PhysXExport_exportPhysX\" 1");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -sv \"PhysXExport_outputUnit\" \"meter\""); // Has no effect
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -iv \"PhysXExport_customScaling\" true");
        if (!status) return false;
        status = MGlobal::executeCommand("optionVar -fv \"PhysXExport_outputScale\" " + centimetersToUIUnitStr);
        if (!status) return false;

        String filePath = mDocumentExporter.getFilename();
        std::stringstream exportCommand;
        exportCommand << "file -force -options \"none=0;\" -type \"PhysX\" -pr";
        if (mDocumentExporter.getSceneGraph()->getExportSelectedOnly()) {
            exportCommand << " -es";
        }
        else {
            exportCommand << " -ea";
        }

        // Generate temp file name
        size_t extPos = filePath.find_last_of('.');
        String filePathNoExt = filePath.substr(0, extPos);
        String tempFilePath = filePathNoExt + ".xml";
        std::ifstream tempFile(tempFilePath.c_str());
        int index = 0;
        while (tempFile.is_open()) {
            std::stringstream s;
            s << index++;
            tempFilePath = filePathNoExt + s.str() + ".xml";
            tempFile.close();
            tempFile.open(tempFilePath.c_str());
        }

        // Set .pxproj as extension but plugin will export a .xml file
        String pxProjPath = tempFilePath;
        extPos = pxProjPath.find_last_of('.');
        pxProjPath.replace(extPos, pxProjPath.length() - extPos, ".PxProj");

        exportCommand << " \"" << pxProjPath << "\";";
        status = MGlobal::executeCommand(exportCommand.str().c_str());
        if (!status) return false;

        // delete .xml file when we are done with it.
        AutoDeleteFile autoDeleteFile(tempFilePath);

        // Read .xml file

        // Is this necessary?
        LIBXML_TEST_VERSION;

        std::ifstream file(tempFilePath.c_str());
        if (!file.is_open()) {
            return false;
        }

        std::stringstream fileContentStream;
        fileContentStream << file.rdbuf();
        std::string fileContent(fileContentStream.str());

        int length = static_cast<int>(fileContent.length());
        xmlDocPtr xml = xmlReadMemory(fileContent.c_str(), length, "noname.xml", NULL, 0);
        if (xml == NULL) {
            return false;
        }

        AutoXmlFreeDoc autoXmlFreeDoc(xml);

        // Init our PhysX xml tree
        mPhysXDoc = xml;

        // Check for duplicated names
        if (!mPhysXDoc->validate()) {
            MGlobal::displayError("Error while exporting PhysX scene: duplicated names found.");
            return false;
        }

        xmlCleanupParser();

        return true;
    }

    bool PhysXExporter::exportPhysicsLibraries()
    {
        // Export to .xml format first using PhysX plugin exporter.
        // Produced file contains information not accessible with Maya API.
        if (!mPhysXDoc) {
            MGlobal::displayError("Can't generate PhysX XML data. PhysX will not be exported.");
            return false;
        }

        bool hasPhysicsScene = false;

        if (!ExportOptions::exportPhysics()) {
            return hasPhysicsScene;
        }

        if (sceneHas(SceneElement::PHYSX_RIGID_BODY, Local) ||
            sceneHas(SceneElement::PHYSX_RIGID_CONSTRAINT, Local)) {
            {
                LibraryPhysicsModels(*this);
            }
            {
                LibraryPhysicsScenes(*this);
            }
            hasPhysicsScene = true;
        }

        return hasPhysicsScene;
    }

    StreamWriter& PhysXExporter::getStreamWriter()
    {
        return mStreamWriter;
    }

    DocumentExporter& PhysXExporter::getDocumentExporter()
    {
        return mDocumentExporter;
    }

	void PhysXExporter::exportTranslationWithoutConversion(const MVector & translation, const String & sid)
	{
		if (!(COLLADABU::Math::Utils::equalsZero(translation.x, mDocumentExporter.getTolerance()) &&
			COLLADABU::Math::Utils::equalsZero(translation.y, mDocumentExporter.getTolerance()) &&
			COLLADABU::Math::Utils::equalsZero(translation.z, mDocumentExporter.getTolerance())))
		{
			Translate(*this, translation, sid);
		}
	}

    void PhysXExporter::exportTranslation(const MVector & translation, const String & sid)
    {
        if (!(COLLADABU::Math::Utils::equalsZero(translation.x, mDocumentExporter.getTolerance()) &&
            COLLADABU::Math::Utils::equalsZero(translation.y, mDocumentExporter.getTolerance()) &&
            COLLADABU::Math::Utils::equalsZero(translation.z, mDocumentExporter.getTolerance())))
        {
            // Convert from Maya internal unit (cm) to export unit
            MVector convertedTranslation;
            convertedTranslation.x = MDistance::internalToUI(translation.x);
            convertedTranslation.y = MDistance::internalToUI(translation.y);
            convertedTranslation.z = MDistance::internalToUI(translation.z);

            Translate(*this, convertedTranslation, sid);
        }
    }

    void PhysXExporter::exportRotation(const MEulerRotation & rotation, const String & sid)
    {
        RotateHelper rotateHelper(rotation);
        const std::vector<std::vector<double> >& rotationMatrix = rotateHelper.getRotationMatrix();
        const std::vector<String>& rotationParameters = rotateHelper.getRotationParameters();

        // Go through the axes for the rotations.
        for (uint i = 0; i < 3; ++i)
        {
            if (!COLLADABU::Math::Utils::equalsZero(rotationMatrix[i][3], mDocumentExporter.getTolerance()))
            {
                exportRotate(MVector(
                    COLLADABU::Math::Utils::equalsZero(rotationMatrix[i][0], mDocumentExporter.getTolerance()) ? 0.0 : rotationMatrix[i][0],
                    COLLADABU::Math::Utils::equalsZero(rotationMatrix[i][1], mDocumentExporter.getTolerance()) ? 0.0 : rotationMatrix[i][1],
                    COLLADABU::Math::Utils::equalsZero(rotationMatrix[i][2], mDocumentExporter.getTolerance()) ? 0.0 : rotationMatrix[i][2]),
                    COLLADABU::Math::Utils::equalsZero(rotationMatrix[i][3], mDocumentExporter.getTolerance()) ? 0.0 : rotationMatrix[i][3],
                    sid + rotationParameters[i]
                    );
            }
        }
    }

    void PhysXExporter::exportAttributes(const MObject & object, const std::set<MString, MStringComp> & attributes)
    {
        AttributeExporter attributeExporter(*this, attributes);
        MFnDependencyNode fnDependencyNode(object);
        AttributeParser::parseAttributes(fnDependencyNode, attributeExporter);
    }

	namespace local
	{
		class ExtraAttributeExporter : public AttributeParser
		{
		public:
			ExtraAttributeExporter(COLLADASW::StreamWriter& streamWriter, const MObject & obj)
				: mStreamWriter(streamWriter)
				, mObject(obj)
			{}

		private:
			COLLADASW::StreamWriter & mStreamWriter;
			MObject mObject;

		protected:
			virtual bool onBeforeAttribute(MFnDependencyNode & node, MObject & attr) override
			{
				MStatus status;
				MFnAttribute fnAttr(attr, &status);
				if (!status) return false;

				MString attrName = fnAttr.name(&status);
				if (!status) return false;

				bool isDynamic = fnAttr.isDynamic(&status);
				if (!status) return false;

				if (!isDynamic)
					return false;

				bool isHidden = fnAttr.isHidden(&status);
				if (!status) return false;

				if (isHidden)
					return false;

				return true;
			}

			template<typename ValueType>
			void exportExtraAttribute(const String & name, const String & type, const ValueType & value)
			{
				mStreamWriter.openElement(CSWC::CSW_ELEMENT_PARAM);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_TYPE, type);
				mStreamWriter.appendValues(value);
				mStreamWriter.closeElement();
			}

			template<typename ValueType>
			void exportExtraAttribute(const String & name, const String & type, const ValueType & value0, const ValueType & value1)
			{
				mStreamWriter.openElement(CSWC::CSW_ELEMENT_PARAM);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_TYPE, type);
				mStreamWriter.appendValues(value0, value1);
				mStreamWriter.closeElement();
			}

			template<typename ValueType>
			void exportExtraAttribute(const String & name, const String & type, const ValueType & value0, const ValueType & value1, const ValueType & value2)
			{
				mStreamWriter.openElement(CSWC::CSW_ELEMENT_PARAM);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_TYPE, type);
				mStreamWriter.appendValues(value0, value1, value2);
				mStreamWriter.closeElement();
			}

			template<typename ValueType>
			void exportExtraAttribute(const String & name, const String & type, const ValueType & value0, const ValueType & value1, const ValueType & value2, const ValueType & value3)
			{
				mStreamWriter.openElement(CSWC::CSW_ELEMENT_PARAM);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_NAME, name);
				mStreamWriter.appendAttribute(CSWC::CSW_ATTRIBUTE_TYPE, type);
				mStreamWriter.appendValues(value0, value1, value2, value3);
				mStreamWriter.closeElement();
			}

			virtual void onBoolean(MPlug & plug, const MString & name, bool value) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_BOOL, value);
			}

			virtual void onInteger(MPlug & plug, const MString & name, int value) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_INT, value);
			}

			virtual void onInteger2(MPlug & plug, const MString & name, int value[2]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_INT2, value[0], value[1]);
			}

			virtual void onInteger3(MPlug & plug, const MString & name, int value[3]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_INT3, value[0], value[1], value[2]);
			}

			virtual void onFloat(MPlug & plug, const MString & name, float value) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_FLOAT, value);
			}

			virtual void onFloat2(MPlug & plug, const MString & name, float value[2]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_FLOAT2, value[0], value[1]);
			}

			virtual void onFloat3(MPlug & plug, const MString & name, float value[3]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_FLOAT3, value[0], value[1], value[2]);
			}

			virtual void onDouble(MPlug & plug, const MString & name, double value) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_DOUBLE, value);
			}

			virtual void onDouble2(MPlug & plug, const MString & name, double value[2]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_DOUBLE2, value[0], value[1]);
			}

			virtual void onDouble3(MPlug & plug, const MString & name, double value[3]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_DOUBLE3, value[0], value[1], value[2]);
			}

			virtual void onDouble4(MPlug & plug, const MString & name, double value[4]) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_DOUBLE4, value[0], value[1], value[2], value[3]);
			}

			virtual void onString(MPlug & plug, const MString & name, const MString & value) override
			{
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_STRING, value.asChar());
			}

			virtual void onEnum(MPlug & plug, const MString & name, int enumValue, const MString & enumName) override
			{
				// TODO export all possible enum values to be able to re-import them?
				exportExtraAttribute(name.asChar(), CSWC::CSW_VALUE_TYPE_STRING, COLLADABU::StringUtils::translateToXML(String(enumName.asChar())));
			}
		};
	}

    void PhysXExporter::exportExtraAttributes(const MObject & object)
    {
        local::ExtraAttributeExporter extraAttributeExporter(mStreamWriter, object);
		MFnDependencyNode node(object);
        AttributeParser::parseAttributes(node, extraAttributeExporter);
    }

    void PhysXExporter::exportMaterialPhysXXML(const MObject& rigidBody)
    {
        PhysXXML::PxMaterial* pxMaterial = findPxMaterial(rigidBody);
        if (pxMaterial) {
            pxMaterial->exportElement(mStreamWriter);
        }
    }

    void PhysXExporter::exportShapePhysXXML(const MObject& rigidBody, const MObject& shape)
    {
        PhysXXML::PxShape* pxShape = findPxShape(rigidBody, shape);
        if (pxShape) {
            pxShape->exportElement(mStreamWriter);
        }
    }

    void PhysXExporter::exportRigidBodyPhysXXML(const MObject& rigidBody)
    {
        int dummy = 0;
        MString simulationType;
        DagHelper::getPlugValue(rigidBody, ATTR_SIMULATION_TYPE, dummy, simulationType);

        if (simulationType == SIMULATION_TYPE_STATIC) {
            PhysXXML::PxRigidStatic* pxRigidStatic = findPxRigidStatic(rigidBody);
            if (pxRigidStatic) {
                pxRigidStatic->exportElement(mStreamWriter);
            }
        }
        else {
            PhysXXML::PxRigidDynamic* pxRigidDynamic = findPxRigidDynamic(rigidBody);
            if (pxRigidDynamic) {
                pxRigidDynamic->exportElement(mStreamWriter);
            }
        }
    }

    void PhysXExporter::exportRigidConstraintPhysXXML(const MObject& rigidConstraint)
    {
        PhysXXML::PxD6Joint* pxConstraint = findPxD6Joint(rigidConstraint);
        if (pxConstraint) {
            pxConstraint->exportElement(mStreamWriter);
        }
    }

    void PhysXExporter::exportRotate(const MVector & axis, double angle, const String & sid)
    {
        Rotate(*this, axis, angle, sid);
    }

    bool PhysXExporter::sceneHas(SceneElement::Type type, PhysXExporter::Filter filter)
    {
        SceneGraph& sceneGraph = *mDocumentExporter.getSceneGraph();
        SceneElementsList& exportNodesTree = *sceneGraph.getExportNodesTree();

        // Parse all/selected DAG nodes
        for (size_t i = 0; i < exportNodesTree.size(); ++i)
        {
            SceneElement& sceneElement = *exportNodesTree[i];
            if (ElementHas(sceneElement, type, filter)) {
                return true;
            }
        }
        return false;
    }

    bool PhysXExporter::ElementHas(const SceneElement & element, SceneElement::Type type, PhysXExporter::Filter filter)
    {
        if (element.getType() == type) {
            switch (filter)
            {
            case All:
                return true;
            case Local:
                return element.getIsLocal();
            case Reference:
                return !element.getIsLocal();
            }
        }

        // Recursive call for all the child elements
        for (uint i = 0; i<element.getChildCount(); ++i)
        {
            const SceneElement& childElement = *element.getChild(i);
            if (ElementHas(childElement, type, filter)) {
                return true;
            }
        }
        return false;
    }

    String PhysXExporter::generateColladaId(const MDagPath & dagPath)
    {
        // Get the maya mesh id.
        String mayaId = mDocumentExporter.dagPathToColladaId(dagPath);

        // Generate a COLLADA id for the new object.
        String colladaId = findColladaId(mayaId);

        // Check, if the unique id for the current geometry is already generated.
        if (!colladaId.empty()) return colladaId;

        // Get the node of the current mesh
        colladaId = mDocumentExporter.dagPathToColladaName(dagPath);

        // Make the id unique and store it in a map.
        colladaId = mIdList.addId(colladaId);
        mMayaIdToColladaId[mayaId] = colladaId;

        return colladaId;
    }

    String PhysXExporter::generateColladaName(const MDagPath & dagPath)
    {
        return mDocumentExporter.dagPathToColladaName(dagPath);
    }

    const String& PhysXExporter::GetDefaultPhysicsModelId()
    {
        return mDefaultPhysicsModelId;
    }

    const String& PhysXExporter::GetDefaultPhysicsSceneId()
    {
        return mDefaultPhysicsSceneId;
    }

    const String& PhysXExporter::GetDefaultInstancePhysicsModelSid()
    {
        return mDefaultInstancePhysicsModelSid;
    }

    const String& PhysXExporter::GetProfile()
    {
        return mProfile;
    }

    const String& PhysXExporter::GetProfileXML()
    {
        return mProfileXML;
    }

	const String & PhysXExporter::GetPhysXProfile()
	{
		return mPhysXProfile;
	}

	namespace local
	{
		class ExtraAttributeParser : public AttributeParser
		{
		public:
			ExtraAttributeParser(const MObject & obj)
				: mHasExtraAttributes(false)
			{}

			bool hasExtraAttributes() const { return mHasExtraAttributes; }

		private:
			bool mHasExtraAttributes;

		protected:
			virtual bool onBeforeAttribute(MFnDependencyNode & node, MObject & attr) override
			{
				MStatus status;
				MFnAttribute fnAttr(attr, &status);
				if (!status) return false;

				MString attrName = fnAttr.name(&status);
				if (!status) return false;

				bool isDynamic = fnAttr.isDynamic(&status);
				if (!status) return false;

				if (!isDynamic)
					return false;

				bool isHidden = fnAttr.isHidden(&status);
				if (!status) return false;

				if (isHidden)
					return false;

				mHasExtraAttributes = true;

				// No need to continue parsing
				return false;
			}
		};
	}

	bool PhysXExporter::HasExtraAttributes(const MObject & object)
	{
		local::ExtraAttributeParser parser(object);
		MFnDependencyNode node(object);
		AttributeParser::parseAttributes(node, parser);
		return parser.hasExtraAttributes();
	}

	String PhysXExporter::CombineModeToCOLLADA(PhysXXML::CombineMode::FlagEnum flag)
	{
		std::map<PhysXXML::CombineMode::FlagEnum, String>::const_iterator it = mCombineModeMap.find(flag);
		if (it != mCombineModeMap.end())
			return it->second;
		return "";
	}

	String PhysXExporter::ShapeFlagsToCOLLADA(const Flags<PhysXXML::ShapeFlags::FlagEnum> & flags)
	{
		return FlagsToCOLLADA(flags, mShapeFlagMap);
	}

	String PhysXExporter::ActorFlagsToCOLLADA(const Flags<PhysXXML::ActorFlags::FlagEnum> & flags)
	{
		return FlagsToCOLLADA(flags, mActorFlagMap);
	}

	String PhysXExporter::RigidBodyFlagsToCOLLADA(const Flags<PhysXXML::RigidBodyFlags::FlagEnum> & flags)
	{
		return FlagsToCOLLADA(flags, mRigidBodyFlagMap);
	}

	String PhysXExporter::ConstraintFlagsToCOLLADA(const Flags<PhysXXML::ConstraintFlags::FlagEnum> & flags)
	{
		return FlagsToCOLLADA(flags, mConstraintFlagMap);
	}

	String PhysXExporter::DriveFlagsToCOLLADA(const Flags<PhysXXML::DriveFlags::FlagEnum> & flags)
	{
		return FlagsToCOLLADA(flags, mDriveFlagMap);
	}

    const String & PhysXExporter::findColladaId(const String & mayaId)
    {
        const StringToStringMap::const_iterator it = mMayaIdToColladaId.find(mayaId);
        if (it != mMayaIdToColladaId.end()) {
            return it->second;
        }
        return EMPTY_STRING;
    }

    void PhysXExporter::GetRigidBodyShapes(const MObject & rigidBody, std::vector<MObject> & shapes)
    {
        MFnDependencyNode rigidBodyNode(rigidBody);
        MPlug plug = rigidBodyNode.findPlug(ATTR_PHYSICS_SHAPES);
        uint numConnectedElements = plug.numConnectedElements();
        
        for (uint i = 0; i < numConnectedElements; ++i)
        {
            MPlug elementPlug = plug.connectionByPhysicalIndex(i);
            MPlugArray connections;
            if (elementPlug.connectedTo(connections, true, false))
            {
                shapes.push_back(connections[0].node());
            }
        }
    }

    // in kg
    double PhysXExporter::GetRigidBodyMass(const MObject & rigidBody)
    {
        int dummy = 0;
        MString overrideMassOrDensity;
        DagHelper::getPlugValue(rigidBody, ATTR_OVERRIDE_MASS_OR_DENSITY, dummy, overrideMassOrDensity);

        double mass = 0.0;
        if (overrideMassOrDensity == OVERRIDE_MASS_OR_DENSITY_MASS)
        {
            // Mass attribute is in kg
            DagHelper::getPlugValue(rigidBody, ATTR_MASS, mass);
        }
        else if (overrideMassOrDensity == OVERRIDE_MASS_OR_DENSITY_DENSITY)
        {
            // Density attribute is in g/cm3
            double density = 0.0;
            DagHelper::getPlugValue(rigidBody, ATTR_DENSITY, density);
            // Volume in cm3
            mass = density * GetRigidBodyVolume(rigidBody);
            // Convert g to kg
            mass /= 1000.0;
        }
        else //if (overrideMassOrDensity == OVERRIDE_MASS_OR_DENSITY_DISABLED)
        {
            std::vector<MObject> shapes;
            GetRigidBodyShapes(rigidBody, shapes);
            for (size_t i = 0; i < shapes.size(); ++i)
            {
                mass += GetShapeMass(shapes[i]);
            }
        }
        return mass;
    }

    // in kg
    double PhysXExporter::GetShapeMass(const MObject & shape)
    {
        int dummy = 0;
        MString useMassOrDensity;
        DagHelper::getPlugValue(shape, ATTR_USE_MASS_OR_DENSITY, dummy, useMassOrDensity);

        double mass = 0.0;
        if (useMassOrDensity == USE_MASS_OR_DENSITY_MASS)
        {
            // Mass in kg
            DagHelper::getPlugValue(shape, ATTR_MASS, mass);
        }
        else //if (useMassOrDensity == USE_MASS_OR_DENSITY_DENSITY)
        {
            // Density in g/cm3
            double density = 0.0;
            DagHelper::getPlugValue(shape, ATTR_DENSITY, density);
            mass = density * GetShapeVolume(shape);
            // Convert g to kg
            mass /= 1000.0;
        }

        return mass;
    }

    // cm3
    double PhysXExporter::GetRigidBodyVolume(const MObject & rigidBody)
    {
        std::vector<MObject> shapes;
        GetRigidBodyShapes(rigidBody, shapes);
        double volume = 0.0;
        for (size_t i = 0; i < shapes.size(); ++i)
        {
            volume += GetShapeVolume(shapes[i]);
        }
        return volume;
    }

    // cm3
    double PhysXExporter::GetShapeVolume(const MObject & shape)
    {
        int dummy = 0;
        MString shapeType;
        DagHelper::getPlugValue(shape, ATTR_SHAPE_TYPE, dummy, shapeType);
        
        double volume = 0.0;

        if (shapeType == SHAPE_TYPE_BOX)
        {
            MVector size = MVector::zero;
            DagHelper::getPlugValue(shape, ATTR_SIZE, size);
            volume = size.x * size.y * size.z;
        }
        else if (shapeType == SHAPE_TYPE_CAPSULE)
        {
            double radius = 0.0;
            double height = 0.0;
            DagHelper::getPlugValue(shape, ATTR_RADIUS, radius);
            DagHelper::getPlugValue(shape, ATTR_HEIGHT, height);
            volume = M_PI * radius * radius * (4.0 / 3.0 * radius + height);
        }
        else if (
            shapeType == SHAPE_TYPE_CLOTH_SPHERES ||
            shapeType == SHAPE_TYPE_SPHERE
            )
        {
            double radius = 0.0;
            DagHelper::getPlugValue(shape, ATTR_RADIUS, radius);
            volume = 4.0 / 3.0 * M_PI * radius * radius * radius;
        }
        else if (shapeType == SHAPE_TYPE_CONVEX_HULL)
        {
            // Get convex mesh (not connected mesh)
            MObject mesh;
            PhysXShape::GetInMesh(shape, mesh);

            // Parse triangles
            MItMeshPolygon itMeshPolygon(mesh);
            for (itMeshPolygon.reset(); !itMeshPolygon.isDone(); itMeshPolygon.next())
            {
                int numTriangles = 0;
                itMeshPolygon.numTriangles(numTriangles);
                for (int triangleIndex = 0; triangleIndex < numTriangles; ++triangleIndex)
                {
                    MPointArray vertexList;
                    MIntArray vertexIndexList;
                    itMeshPolygon.getTriangle(triangleIndex, vertexList, vertexIndexList);
                    
                    MPoint a, b, c;
                    a = vertexList[0];
                    b = vertexList[1];
                    c = vertexList[2];

                    MVector vab, vbc, vca;
                    vab = b - a;
                    vbc = c - b;
                    vca = a - c;
                    double ab, bc, ca;
                    ab = vab.length();
                    bc = vbc.length();
                    ca = vca.length();
                    // Use Heron's formula to calculate triangular pyramid signed volume
                    // base triangle perimeter
                    double p = 0.5 * (ab + bc + ca);
                    // base triangle area
                    double area = sqrt(p * (p-ab) * (p-bc) * (p-ca));
                    MVector vac;
                    vac = c - a;
                    // triangle plane normal
                    MVector n = vab ^ vac;
                    n.normalize();
                    // triangle plane equation constant
                    double d = -a.x*n.x - a.y*n.y - a.z*n.z;
                    // pyramid height
                    double h = d;
                    // pyramid volume
                    double subVolume = 1.0/3.0 * area * h;
                    subVolume *= -1;
                    // add to total volume
                    volume += subVolume;
                }
            }
        }
        else
        {
            MVector bbSize = MVector::zero;
            DagHelper::getPlugValue(shape, ATTR_BOUNDING_BOX_SIZE, bbSize);
            volume = bbSize.x * bbSize.y * bbSize.z;
        }

        return volume;
    }

    MStatus PhysXExporter::GetPluggedObject(const MObject & object, const MString & attribute, MObject & pluggedObject)
    {
        MStatus status;

        MFnDependencyNode node(object, &status);
        if (!status) return status;

        MPlug plug = node.findPlug(attribute, &status);
        if (!status) return status;

        MPlugArray plugArray;
        bool hasConnection = plug.connectedTo(plugArray, true, false, &status);
        if (!status) return status;
        if (hasConnection)
        {
            MPlug externalPlug = plugArray[0];
            bool externalPlugNull = externalPlug.isNull(&status);
            if (!status) return status;
            if (!externalPlugNull)
            {
                pluggedObject = externalPlug.node(&status);
                if (!status) return status;
            }
        }
        return MS::kSuccess;
    }

    MStatus PhysXExporter::getMeshURI(const MObject & mesh, URI & meshURI)
    {
        MStatus status;

        MFnDagNode meshDagNode(mesh, &status);
        if (!status) return status;

        MDagPath meshDagPath;
        status = meshDagNode.getPath(meshDagPath);
        if (!status) return status;

        String geometryId = mDocumentExporter.getGeometryExporter()->generateColladaMeshId(meshDagPath);
        meshURI = getDocumentExporter().getVisualSceneExporter()->getSceneElementURI(meshDagPath, geometryId);

        return status;
    }
}
