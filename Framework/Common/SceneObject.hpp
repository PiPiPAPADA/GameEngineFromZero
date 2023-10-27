#pragma once
#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "Guid.hpp"
#include "Image.hpp"
#include "portable.hpp"
#include "geommath.hpp"
namespace My{
    namespace details
    {
        constexpr int32_t i32(const char* s,int32_t v){
            return *s ? i32(s+1,v*256+*s) : v;
        }
    } // namespace details
    constexpr int32_t operator "" _i32(const char* s, size_t) {
        return details::i32(s, 0);
    }

    ENUM(SceneObjectType) {
        kSceneObjectTypeMesh    =   "MESH"_i32,
        kSceneObjectTypeMaterial=   "MATL"_i32,
        kSceneObjectTypeTexture =   "TXTU"_i32,
        kSceneObjectTypeLight   =   "LGHT"_i32,
        kSceneObjectTypeCamera  =   "CAMR"_i32,
        kSceneObjectTypeAnimator=   "ANIM"_i32,
        kSceneObjectTypeClip    =   "CLIP"_i32,
        kSceneObjectTypeVertexArray = "VARR"_i32,
        kSceneObjectTypeIndexArray = "VARR"_i32,
        kSceneObjectTypeGeometry = "GEOM"_i32,
    };

    std::ostream& operator<<(std::ostream& out, SceneObjectType type);
    // std::ostream& operator<<(std::ostream& out, SceneObjectType type){
    //     int32_t n = static_cast<int32_t>(type);
    //     n = endian_net_unsigned_int<int32_t>(n);
    //     char*c = reinterpret_cast<char*>(&n);
    //     for(int i=0;i<sizeof(int32_t);i++){
    //         out<<*c++;
    //     }
    //     return out;
    // }

    using namespace xg;
    class BaseSceneObject{
        protected:
            Guid m_Guid;
            SceneObjectType m_type;
        protected:
            BaseSceneObject(SceneObjectType type): m_type(type) {m_Guid = newGuid();}
            BaseSceneObject(Guid& guid,SceneObjectType type) :m_type(type),m_Guid(guid) {}
            BaseSceneObject(Guid&& guid,SceneObjectType type) :m_type(type),m_Guid(std::move(guid)){}
            BaseSceneObject(BaseSceneObject&& obj) :m_type(obj.m_type),m_Guid(std::move(obj.m_Guid)){}
            BaseSceneObject& operator=(BaseSceneObject&& obj) {this->m_Guid=std::move(obj.m_Guid);this->m_type = obj.m_type; return *this;}
            virtual ~BaseSceneObject() {};

        private:
            BaseSceneObject() = delete;
            BaseSceneObject(BaseSceneObject& obj) = delete;
            BaseSceneObject& operator=(BaseSceneObject& obj) = delete;
        
        public:
            const Guid& GetGuid() const {return m_Guid;}
            const SceneObjectType GetType() const {return m_type;}
        friend std::ostream& operator<<(std::ostream& out, const BaseSceneObject& obj);
        // friend std::ostream& operator<<(std::ostream& out, const BaseSceneObject& obj){
        //     out<<"SceneObject"<<std::endl;
        //     out<<"-----------"<<std::endl;
        //     out<<"GUID: "<<obj.m_Guid<<std::endl;
        //     out<<"Type: "<<obj.m_type<<std::endl;
        //     return out;
        // }   
    };

    ENUM(VertexDataType){
        kVertexDataTypeFloat1    = "FLT1"_i32,
        kVertexDataTypeFloat2    = "FLT2"_i32,
        kVertexDataTypeFloat3    = "FLT3"_i32,
        kVertexDataTypeFloat4    = "FLT4"_i32,
        kVertexDataTypeDouble1   = "DUB1"_i32,
        kVertexDataTypeDouble2   = "DUB2"_i32,
        kVertexDataTypeDouble3   = "DUB3"_i32,
        kVertexDataTypeDouble4   = "DUB4"_i32
    };

    std::ostream& operator<<(std::ostream& out, VertexDataType type);
    // std::ostream& operator<<(std::ostream& out, VertexDataType type){
    //     int32_t n = static_cast<int32_t>(type);
    //     n = endian_net_unsigned_int<int32_t>(n);
    //     char* c = reinterpret_cast<char*> (&n);

    //     for(size_t i=0; i< sizeof(int32_t);i++)
    //         out<<*c++;
    //     return out;
    // }

    class SceneObjectVertexArray{
    protected:
        const std::string m_Attribute;
        const uint32_t m_MorphTargetIndex;
        const VertexDataType m_DataType;

        const void* m_pDataFloat;

        // union 
        // {
        //     float*  m_pDataFloat;
        //     double* m_pDataDouble;
        // };
        const size_t m_szData;
    public:
        SceneObjectVertexArray(const char* attr="",const uint32_t morph_index=0, const VertexDataType data_type=VertexDataType::kVertexDataTypeFloat3,const void* data = nullptr, const size_t data_size=0):  
            m_Attribute(attr),m_MorphTargetIndex(morph_index),m_DataType(data_type),m_pDataFloat(data),m_szData(data_size) {};
        SceneObjectVertexArray(SceneObjectVertexArray& arr) = default;
        SceneObjectVertexArray(SceneObjectVertexArray&& arr)= default;
        const std::string& GetAttributeName() const { return m_Attribute; };
            VertexDataType GetDataType() const { return m_DataType; };
            size_t GetDataSize() const 
            { 
                size_t size = m_szData;

                switch(m_DataType) {
                    case VertexDataType::kVertexDataTypeFloat1:
                    case VertexDataType::kVertexDataTypeFloat2:
                    case VertexDataType::kVertexDataTypeFloat3:
                    case VertexDataType::kVertexDataTypeFloat4:
                        size *= sizeof(float);
                        break;
                    case VertexDataType::kVertexDataTypeDouble1:
                    case VertexDataType::kVertexDataTypeDouble2:
                    case VertexDataType::kVertexDataTypeDouble3:
                    case VertexDataType::kVertexDataTypeDouble4:
                        size *= sizeof(double);
                        break;
                    default:
                        size = 0;
                        assert(0);
                        break;
                }

                return size;
            }; 
            const void* GetData() const { return m_pDataFloat; };
            size_t GetVertexCount() const
            {
                size_t size = m_szData;

                switch(m_DataType) {
                    case VertexDataType::kVertexDataTypeFloat1:
                        size /= 1;
                        break;
                    case VertexDataType::kVertexDataTypeFloat2:
                        size /= 2;
                        break;
                    case VertexDataType::kVertexDataTypeFloat3:
                        size /= 3;
                        break;
                    case VertexDataType::kVertexDataTypeFloat4:
                        size /= 4;
                        break;
                    case VertexDataType::kVertexDataTypeDouble1:
                        size /= 1;
                        break;
                    case VertexDataType::kVertexDataTypeDouble2:
                        size /= 2;
                        break;
                    case VertexDataType::kVertexDataTypeDouble3:
                        size /= 3;
                        break;
                    case VertexDataType::kVertexDataTypeDouble4:
                        size /= 4;
                        break;
                    default:
                        size = 0;
                        assert(0);
                        break;
                }

                return size;
            }
    friend std::ostream& operator<<(std::ostream& out, const SceneObjectVertexArray& obj);
    // friend std::ostream& operator<<(std::ostream& out, const SceneObjectVertexArray& obj)
    // {
    //         out << "Attribute: " << obj.m_Attribute << std::endl;
    //         out << "Morph Target Index: 0x" << obj.m_MorphTargetIndex << std::endl;
    //         out << "Data Type: " << obj.m_DataType << std::endl;
    //         out << "Data Size: 0x" << obj.m_szData << std::endl;
    //         out << "Data: ";
    //         for(size_t i = 0; i < obj.m_szData; i++)
    //         {
    //             out << *(reinterpret_cast<const float*>(obj.m_pDataFloat) + i) << ' ';
    //         }

    //         return out;
    // }
    
    };

    ENUM(IndexDataType){
        kIndexDataTypeInt8  = "I8 "_i32,
        kIndexDataTypeInt16 = "I16 "_i32,
        kIndexDataTypeInt32 = "I32 "_i32,
        kIndexDataTypeInt64 = "I64 "_i32,
    };
    std::ostream& operator<<(std::ostream& out, IndexDataType type);
    // std::ostream& operator<<(std::ostream& out, IndexDataType type)
    // {
    //     int32_t n = static_cast<int32_t>(type);
    //     n = endian_net_unsigned_int<int32_t>(n);
    //     char* c = reinterpret_cast<char*>(&n);
         
    //     for (size_t i = 0; i < sizeof(int32_t); i++) {
    //         out << *c++;
    //     }

    //     return out;
    // }
    class  SceneObjectIndexArray {
    protected:
        const uint32_t m_nMaterialIndex;
        const size_t m_szRestartIndex;
        const IndexDataType m_DataType;

        const void* m_pData;
        const size_t m_szData;
    public:
       SceneObjectIndexArray(const uint32_t material_index = 0, const size_t restart_index = 0, const IndexDataType data_type = IndexDataType::kIndexDataTypeInt16, const void* data = nullptr, const size_t data_size = 0) 
                : m_nMaterialIndex(material_index), m_szRestartIndex(restart_index), m_DataType(data_type), m_pData(data), m_szData(data_size) {};
            SceneObjectIndexArray(SceneObjectIndexArray& arr) = default;
            SceneObjectIndexArray(SceneObjectIndexArray&& arr) = default;
         const IndexDataType GetIndexType() const { return m_DataType; };
            const void* GetData() const { return m_pData; };
            size_t GetDataSize() const 
            { 
                size_t size = m_szData;

                switch(m_DataType) {
                    case IndexDataType::kIndexDataTypeInt8:
                        size *= sizeof(int8_t);
                        break;
                    case IndexDataType::kIndexDataTypeInt16:
                        size *= sizeof(int16_t);
                        break;
                    case IndexDataType::kIndexDataTypeInt32:
                        size *= sizeof(int32_t);
                        break;
                    case IndexDataType::kIndexDataTypeInt64:
                        size *= sizeof(int64_t);
                        break;
                    default:
                        size = 0;
                        assert(0);
                        break;
                }

                return size;
            };

            size_t GetIndexCount() const
            {
                return m_szData;
            }
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectIndexArray& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectIndexArray& obj)
        // {
        //     out << "Material Index: 0x" << obj.m_nMaterialIndex << std::endl;
        //     out << "Restart Index: 0x" << obj.m_szRestartIndex << std::endl;
        //     out << "Data Type: " << obj.m_DataType << std::endl;
        //     out << "Data Size: 0x" << obj.m_szData << std::endl;
        //     out << "Data: ";
        //     for(size_t i = 0; i < obj.m_szData; i++)
        //     {
        //         switch(obj.m_DataType)
        //         {
        //             case IndexDataType::kIndexDataTypeInt8:
        //                 out << "0x" << *(reinterpret_cast<const uint8_t*>(obj.m_pData) + i) << ' ';;
        //                 break;
        //             case IndexDataType::kIndexDataTypeInt16:
        //                 out << "0x" << *(reinterpret_cast<const uint16_t*>(obj.m_pData) + i) << ' ';;
        //                 break;
        //             case IndexDataType::kIndexDataTypeInt32:
        //                 out << "0x" << *(reinterpret_cast<const uint32_t*>(obj.m_pData) + i) << ' ';;
        //                 break;
        //             case IndexDataType::kIndexDataTypeInt64:
        //                 out << "0x" << *(reinterpret_cast<const uint64_t*>(obj.m_pData) + i) << ' ';;
        //                 break;
        //             default:
        //                 ;
        //         }
        //     }


        //     return out;
        // }
    };

    typedef enum _PrimitiveType : int32_t {
		kPrimitiveTypeNone = "NONE"_i32,        ///< No particular primitive type.
		kPrimitiveTypePointList = "PLST"_i32,   ///< For N>=0, vertex N renders a point.
		kPrimitiveTypeLineList = "LLST"_i32,    ///< For N>=0, vertices [N*2+0, N*2+1] render a line.
		kPrimitiveTypeLineStrip = "LSTR"_i32,   ///< For N>=0, vertices [N, N+1] render a line.
		kPrimitiveTypeTriList = "TLST"_i32,     ///< For N>=0, vertices [N*3+0, N*3+1, N*3+2] render a triangle.
		kPrimitiveTypeTriFan = "TFAN"_i32,      ///< For N>=0, vertices [0, (N+1)%M, (N+2)%M] render a triangle, where M is the vertex count.
		kPrimitiveTypeTriStrip = "TSTR"_i32,    ///< For N>=0, vertices [N*2+0, N*2+1, N*2+2] and [N*2+2, N*2+1, N*2+3] render triangles.
		kPrimitiveTypePatch = "PACH"_i32,       ///< Used for tessellation.
		kPrimitiveTypeLineListAdjacency = "LLSA"_i32,       ///< For N>=0, vertices [N*4..N*4+3] render a line from [1, 2]. Lines [0, 1] and [2, 3] are adjacent to the rendered line.
		kPrimitiveTypeLineStripAdjacency = "LSTA"_i32,      ///< For N>=0, vertices [N+1, N+2] render a line. Lines [N, N+1] and [N+2, N+3] are adjacent to the rendered line.
		kPrimitiveTypeTriListAdjacency = "TLSA"_i32,        ///< For N>=0, vertices [N*6..N*6+5] render a triangle from [0, 2, 4]. Triangles [0, 1, 2] [4, 2, 3] and [5, 0, 4] are adjacent to the rendered triangle.
		kPrimitiveTypeTriStripAdjacency = "TSTA"_i32,       ///< For N>=0, vertices [N*4..N*4+6] render a triangle from [0, 2, 4] and [4, 2, 6]. Odd vertices Nodd form adjacent triangles with indices min(Nodd+1,Nlast) and max(Nodd-3,Nfirst).
		kPrimitiveTypeRectList = "RLST"_i32,    ///< For N>=0, vertices [N*3+0, N*3+1, N*3+2] render a screen-aligned rectangle. 0 is upper-left, 1 is upper-right, and 2 is the lower-left corner.
		kPrimitiveTypeLineLoop = "LLOP"_i32,    ///< Like <c>kPrimitiveTypeLineStrip</c>, but the first and last vertices also render a line.
		kPrimitiveTypeQuadList = "QLST"_i32,    ///< For N>=0, vertices [N*4+0, N*4+1, N*4+2] and [N*4+0, N*4+2, N*4+3] render triangles.
		kPrimitiveTypeQuadStrip = "QSTR"_i32,   ///< For N>=0, vertices [N*2+0, N*2+1, N*2+3] and [N*2+0, N*2+3, N*2+2] render triangles.
		kPrimitiveTypePolygon = "POLY"_i32,     ///< For N>=0, vertices [0, N+1, N+2] render a triangle.
	} PrimitiveType;

    std::ostream& operator<<(std::ostream& out, PrimitiveType type);
    // std::ostream& operator<<(std::ostream& out, PrimitiveType type)
    // {
    //     int32_t n = static_cast<int32_t>(type);
    //     n = endian_net_unsigned_int<int32_t>(n);
    //     char* c = reinterpret_cast<char*>(&n);
         
    //     for (size_t i = 0; i < sizeof(int32_t); i++) {
    //         out << *c++;
    //     }

    //     return out;
    // }
  

    class SceneObjectMesh : public BaseSceneObject{
        protected:
            std::vector<SceneObjectIndexArray> m_IndexArray;
            std::vector<SceneObjectVertexArray> m_VertexArray;
            PrimitiveType m_PrimitiveType;

            bool m_bVisible ;
            bool m_bShadow  ;
            bool m_bMotionBlur ;
        public:
            SceneObjectMesh(bool visible = true, bool shadow = true, bool motion_blur = true) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh), m_bVisible(visible), m_bShadow(shadow), m_bMotionBlur(motion_blur){};
            void AddIndexArray(SceneObjectIndexArray&& array) {m_IndexArray.push_back(std::move(array));}
            void AddVertexArray(SceneObjectVertexArray&& array) {m_VertexArray.push_back(std::move(array));}
            SceneObjectMesh(SceneObjectMesh&& mesh)
                : BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh), 
                m_IndexArray(std::move(mesh.m_IndexArray)),
                m_VertexArray(std::move(mesh.m_VertexArray)),
                m_PrimitiveType(mesh.m_PrimitiveType),
                m_bVisible(mesh.m_bVisible),
                m_bShadow(mesh.m_bShadow),
                m_bMotionBlur(mesh.m_bMotionBlur)
            {
            };
            void SetPrimitiveType(PrimitiveType type) { m_PrimitiveType = type;}
            size_t GetIndexGroupCount() const { return m_IndexArray.size(); };
            size_t GetIndexCount(const size_t index) const { return (m_IndexArray.empty()? 0 : m_IndexArray[index].GetIndexCount()); };
            size_t GetVertexCount() const { return (m_VertexArray.empty()? 0 : m_VertexArray[0].GetVertexCount()); };
            size_t GetVertexPropertiesCount() const { return m_VertexArray.size(); }; 
            const SceneObjectVertexArray& GetVertexPropertyArray(const size_t index) const { return m_VertexArray[index]; };
            const SceneObjectIndexArray& GetIndexArray(const size_t index) const { return m_IndexArray[index]; };
            const PrimitiveType& GetPrimitiveType() { return m_PrimitiveType; };
        
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj){
        //     out << static_cast<const BaseSceneObject&>(obj) << std::endl;
        //     out << "Primitive Type: " << obj.m_PrimitiveType << std::endl;
        //     out << "This mesh contains 0x" << obj.m_VertexArray.size() << " vertex properties." << std::endl;
        //     for (size_t i = 0; i < obj.m_VertexArray.size(); i++) {
        //         out << obj.m_VertexArray[i] << std::endl;
        //     }
        //     out << "This mesh contains 0x" << obj.m_IndexArray.size() << " index arrays." << std::endl;
        //     for (size_t i = 0; i < obj.m_IndexArray.size(); i++) {
        //         out << obj.m_IndexArray[i] << std::endl;
        //     }
        //     out << "Visible: " << obj.m_bVisible << std::endl;
        //     out << "Shadow: " << obj.m_bShadow << std::endl;
        //     out << "Motion Blur: " << obj.m_bMotionBlur << std::endl;

        //     return out;
        // }
    };

     class SceneObjectTexture : public BaseSceneObject
    {
        protected:
            uint32_t m_nTexCoordIndex;
            std::string m_Name;
            std::shared_ptr<Image> m_pImage;
            std::vector<Matrix4X4f> m_Transforms;

        public:
            SceneObjectTexture() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(0) {};
            SceneObjectTexture(std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(0), m_Name(name) {};
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(coord_index), m_pImage(image) {};
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>&& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_nTexCoordIndex(coord_index), m_pImage(std::move(image)) {};
            SceneObjectTexture(SceneObjectTexture&) = default;
            SceneObjectTexture(SceneObjectTexture&&) = default;
            void SetName(std::string& name) { m_Name = name; };
            void SetName(std::string&& name) { m_Name = std::move(name); }
            void AddTransform(Matrix4X4f& matrix) { m_Transforms.push_back(matrix); };

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectTexture& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectTexture& obj)
        // {
        //     out << static_cast<const BaseSceneObject&>(obj) << std::endl;
        //     out << "Coord Index: " << obj.m_nTexCoordIndex << std::endl;
        //     out << "Name: " << obj.m_Name << std::endl;
        //     if (obj.m_pImage)
        //         out << "Image: " << *obj.m_pImage << std::endl;

        //     return out;
        // }
    };

   
    template <typename T>
    struct ParameterValueMap
    {
        T Value;
        std::shared_ptr<SceneObjectTexture> ValueMap;

        ParameterValueMap() = default;

        ParameterValueMap(const T value) : Value(value) {};
        ParameterValueMap(const std::shared_ptr<SceneObjectTexture>& value) : ValueMap(value) {};

        ParameterValueMap(const ParameterValueMap& rhs) = default;

        ParameterValueMap(ParameterValueMap&& rhs) = default;

        ParameterValueMap& operator=(const ParameterValueMap& rhs) = default;
        ParameterValueMap& operator=(ParameterValueMap&& rhs) = default;
        ParameterValueMap& operator=(const std::shared_ptr<SceneObjectTexture>& rhs) 
        {
            ValueMap = rhs;
            return *this;
        };

        ~ParameterValueMap() = default;

        friend std::ostream& operator<<(std::ostream& out, const ParameterValueMap& obj)
        {
            out << "Parameter Value: " << obj.Value << std::endl;
            if (obj.ValueMap) {
                out << "Parameter Map: " << *obj.ValueMap << std::endl;
            }

            return out;
        }
    };

    typedef ParameterValueMap<Vector4f> Color;
    typedef ParameterValueMap<Vector3f> Normal;
    typedef ParameterValueMap<float>    Parameter;

    class SceneObjectMaterial : public BaseSceneObject
    {
        protected:
            std::string m_Name;
            Color       m_BaseColor;
            Parameter   m_Metallic;
            Parameter   m_Roughness;
            Normal      m_Normal;
            Parameter   m_Specular;
            Parameter   m_AmbientOcclusion;

        public:
            SceneObjectMaterial(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name) {};
            SceneObjectMaterial(std::string&& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(std::move(name)) {};
           
            SceneObjectMaterial(const std::string& name = "", Color&& base_color = Vector4f(1.0f), Parameter&& metallic = 0.0f, Parameter&& roughness = 0.0f, Normal&& normal = Vector3f(0.0f, 0.0f, 1.0f), Parameter&& specular = 0.0f, Parameter&& ao = 0.0f) 
                : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial),m_Name(name), m_BaseColor(std::move(base_color)), m_Metallic(std::move(metallic)), m_Roughness(std::move(roughness)), m_Normal(std::move(normal)), m_Specular(std::move(specular)), m_AmbientOcclusion(std::move(ao)) {};
            void SetName(const std::string& name) { m_Name = name; };
            void SetName(std::string&& name) { m_Name = std::move(name); };
            void SetColor(std::string& attrib, Vector4f& color) 
            { 
                if(attrib == "deffuse") {
                    m_BaseColor = Color(color); 
                }
            };

            void SetParam(std::string& attrib, float param) 
            { 
            };

            void SetTexture(std::string& attrib, std::string& textureName) 
            { 
                if(attrib == "diffuse") {
                    m_BaseColor = std::make_shared<SceneObjectTexture>(textureName); 
                }
            };

            void SetTexture(std::string& attrib, std::shared_ptr<SceneObjectTexture>& texture) 
            { 
                if(attrib == "diffuse") {
                    m_BaseColor = texture; 
                }
            };
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj);       
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj){            
        //     out <<static_cast<const BaseSceneObject&>(obj)<<std::endl;
        //     out << "Name: " << obj.m_Name << std::endl;
        //     out << "Albedo: " << obj.m_BaseColor << std::endl;
        //     out << "Metallic: " << obj.m_Metallic << std::endl;
        //     out << "Roughness: " << obj.m_Roughness << std::endl;
        //     out << "Normal: " << obj.m_Normal << std::endl;
        //     out << "Specular: " << obj.m_Specular << std::endl;
        //     out << "Ambient Occlusion:: " << obj.m_AmbientOcclusion << std::endl;

        //     return out;
        // }
    };

    class SceneObjectGeometry : public BaseSceneObject{
        protected:
             std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;
            bool        m_bVisible;
			bool        m_bShadow;
			bool        m_bMotionBlur;
        public:
            void SetVisibility(bool visible) { m_bVisible = visible; };
			const bool Visible() { return m_bVisible; };
			void SetIfCastShadow(bool shadow) { m_bShadow = shadow; };
			const bool CastShadow() { return m_bShadow; };
			void SetIfMotionBlur(bool motion_blur) { m_bMotionBlur = motion_blur; };
			const bool MotionBlur() { return m_bMotionBlur; };
            void AddMesh(std::shared_ptr<SceneObjectMesh>& mesh) { m_Mesh.push_back(std::move(mesh)); };
            SceneObjectGeometry(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeGeometry) {};
            const std::weak_ptr<SceneObjectMesh> GetMesh() {return (m_Mesh.empty()? nullptr : m_Mesh[0]);};
            const std::weak_ptr<SceneObjectMesh> GetMeshLOD(size_t lod) {return (lod < m_Mesh.size() ? m_Mesh[lod] : nullptr);};
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj)
        // {
        //     auto count = obj.m_Mesh.size();
        //     for(decltype(count) i = 0; i < count; i++) {
        //         out << "Mesh: " << *obj.m_Mesh[i] << std::endl;
        //     }

        //     return out;
        // }
    };

    //Intensity, Distance
    typedef float (*AttenFunc)(float,float);
    float DefaultAttenFunc(float intensity, float distance);
    // float DefaultAttenFunc(float intensity, float distance){
    //     return intensity/(1+distance);
    // }

    class SceneObjectLight : public BaseSceneObject{
    protected:
        Color  m_LightColor;
        float  m_Intensity;
        AttenFunc m_LightAttenuation;
        float  m_fNearClipDistance;
        float  m_fFarClipDistance;
        bool   m_bCastShadows;
        std::string m_strTexture;
    public:
        void SetIfCastShadow(bool shadow) { m_bCastShadows = shadow; };

            void SetColor(std::string& attrib, Vector4f& color) 
            { 
                if(attrib == "light") {
                    m_LightColor = Color(color); 
                }
            };

            void SetParam(std::string& attrib, float param) 
            { 
                if(attrib == "intensity") {
                    m_Intensity = param; 
                }
            };

            void SetTexture(std::string& attrib, std::string& textureName) 
            { 
                if(attrib == "projection") {
                    m_strTexture = textureName;
                }
            };
            void SetAttenuation(AttenFunc func){
                m_LightAttenuation = func;
            }
        const Color& GetColor() { return m_LightColor; };
        float GetIntensity() { return m_Intensity; };
    protected:
        SceneObjectLight(Color&& color = Vector4f(1.0f),float intensity =10.0f, AttenFunc atten_func = DefaultAttenFunc, float near_clip = 1.0f, float far_clip = 100.0f, bool cast_shadows = false ) : BaseSceneObject(SceneObjectType::kSceneObjectTypeLight),m_LightColor(color),m_Intensity(intensity),m_LightAttenuation(atten_func), m_fNearClipDistance(near_clip), m_fFarClipDistance(far_clip), m_bCastShadows(cast_shadows) {};

    friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj);
    // friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj){
    //         out << static_cast<const BaseSceneObject&>(obj) << std::endl;
    //         out << "Color: " << obj.m_LightColor << std::endl;
    //         out << "Intensity: " << obj.m_Intensity << std::endl;
    //         out << "Near Clip Distance: " << obj.m_fNearClipDistance << std::endl;
    //         out << "Far Clip Distance: " << obj.m_fFarClipDistance << std::endl;
    //         out << "Cast Shadows: " << obj.m_bCastShadows << std::endl;

    //         return out;
    // }

    };
     class SceneObjectInfiniteLight : public SceneObjectLight
    {
        public:
            using SceneObjectLight::SceneObjectLight;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectInfiniteLight& obj);
    };


    class SceneObjectOmnLight : public SceneObjectLight{
        public:
            using SceneObjectLight::SceneObjectLight;
        friend std::ostream& operator<<(std::ostream&  out, const SceneObjectOmnLight& obj);
        // friend std::ostream& operator<<(std::ostream&  out, const SceneObjectOmnLight& obj){
        //     out<< static_cast<const SceneObjectLight&>(obj) <<std::endl;
        //     out<<"Light Type: Omn "<<std::endl;

        //     return out;
        // }
    };

    class SceneObjectSpotLight : public SceneObjectLight{
        protected:
            float m_fConeAngle;
            float m_fPenumbraAngle;
        public:
            
           SceneObjectSpotLight(Color&& color = Vector4f(1.0f),float intensity = 10.0f, AttenFunc atten_func = DefaultAttenFunc, float near_clip = 1.0f, float far_clip = 100.0f, bool cast_shadows = false, float cone_angle = PI / 4.0f, float penumbra_angle = PI / 3.0f) : SceneObjectLight(std::move(color), intensity, atten_func, near_clip, far_clip, cast_shadows), m_fConeAngle(cone_angle), m_fPenumbraAngle(penumbra_angle) {};
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj){
        //     out << static_cast<const SceneObjectLight&>(obj) << std::endl;
        //     out << "Light Type: Spot" << std::endl;
        //     out << "Cone Angle: " << obj.m_fConeAngle << std::endl;
        //     out << "Penumbra Angle: " << obj.m_fPenumbraAngle << std::endl;

        //     return out;
        // }
    };  

    class SceneObjectCamera : public BaseSceneObject{
        protected:
            float m_fAspect;
            float m_fNearClipDistance;
            float m_fFarClipDistance;
        public:
            void SetColor(std::string& attrib, Vector4f& color) 
            { 
                // TODO: extension
            };

            void SetParam(std::string& attrib, float param) 
            { 
                if(attrib == "near") {
                    m_fNearClipDistance = param; 
                }
                else if(attrib == "far") {
                    m_fFarClipDistance = param; 
                }
            };

            void SetTexture(std::string& attrib, std::string& textureName) 
            { 
                // TODO: extension
            };

            float GetNearClipDistance() const { return m_fNearClipDistance;}
            float GetFarClipDistance() const {return m_fFarClipDistance;}

            SceneObjectCamera(float aspect = 16.0f / 9.0f, float near_clip = 1.0f, float far_clip = 100.0f) : BaseSceneObject(SceneObjectType::kSceneObjectTypeCamera), m_fAspect(aspect), m_fNearClipDistance(near_clip), m_fFarClipDistance(far_clip) {};
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectCamera& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectCamera& obj)
        // {
        //     out << static_cast<const BaseSceneObject&>(obj) << std::endl;
        //     out << "Aspect: " << obj.m_fAspect << std::endl;
        //     out << "Near Clip Distance: " << obj.m_fNearClipDistance << std::endl;
        //     out << "Far Clip Distance: " << obj.m_fFarClipDistance << std::endl;

        //     return out;
        // }
    };

    class SceneObjectOrthogonalCamera : public SceneObjectCamera{
        public:
            using SceneObjectCamera::SceneObjectCamera;
        
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectOrthogonalCamera& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectOrthogonalCamera& obj){
        //     out << static_cast<const SceneObjectCamera&>(obj) << std::endl;
        //     out << "Camera Type: Orthogonal" << std::endl;

        //     return out;
        // }
    };

    class SceneObjectPerspectiveCamera: public SceneObjectCamera{
        protected:
            float m_fFov;
        public:
          void SetParam(std::string& attrib, float param) 
            { 
                // TODO: handle fovx, fovy
                if(attrib == "fov") {
                    m_fFov = param; 
                }
                SceneObjectCamera::SetParam(attrib,param);
            };
             SceneObjectPerspectiveCamera(float aspect = 16.0f / 9.0f, float near_clip = 1.0f, float far_clip = 100.0f, float fov = PI / 2.0) : SceneObjectCamera(aspect, near_clip, far_clip), m_fFov(fov) {};
            float GetFov() const {return m_fFov;};

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj)
        // {
        //     out << static_cast<const SceneObjectCamera&>(obj) << std::endl;
        //     out << "Camera Type: Perspective" << std::endl;
        //     out << "FOV: " << obj.m_fFov<< std::endl;

        //     return out;
        // }
    };

      class SceneObjectTransform
    {
        protected:
            Matrix4X4f m_matrix;
            bool m_bSceneObjectOnly;

        public:
            SceneObjectTransform() { BuildIdentityMatrix(m_matrix); m_bSceneObjectOnly = false; };

            SceneObjectTransform(const Matrix4X4f& matrix, const bool object_only = false) { m_matrix = matrix; m_bSceneObjectOnly = object_only; };

            operator Matrix4X4f() { return m_matrix; };
            operator const Matrix4X4f() const { return m_matrix; };

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectTransform& obj);
        // friend std::ostream& operator<<(std::ostream& out, const SceneObjectTransform& obj)
        // {
        //     out << "Transform Matrix: " << obj.m_matrix << std::endl;
        //     out << "Is Object Local: " << obj.m_bSceneObjectOnly << std::endl;

        //     return out;
        // }
    };

    class SceneObjectTranslation : public SceneObjectTransform
    {
        public:
            SceneObjectTranslation(const char axis, const float amount)  
            { 
                switch (axis) {
                    case 'x':
                        MatrixTranslation(m_matrix, amount, 0.0f, 0.0f);
                        break;
                    case 'y':
                        MatrixTranslation(m_matrix, 0.0f, amount, 0.0f);
                        break;
                    case 'z':
                        MatrixTranslation(m_matrix, 0.0f, 0.0f, amount);
                        break;
                    default:
                        assert(0);
                }
            }

            SceneObjectTranslation(const float x, const float y, const float z) 
            {
                MatrixTranslation(m_matrix, x, y, z);
            }
    };

    class SceneObjectRotation : public SceneObjectTransform
    {
        public:
            SceneObjectRotation(const char axis, const float theta)
            {
                switch (axis) {
                    case 'x':
                        MatrixRotationX(m_matrix, theta);
                        break;
                    case 'y':
                        MatrixRotationY(m_matrix, theta);
                        break;
                    case 'z':
                        MatrixRotationZ(m_matrix, theta);
                        break;
                    default:
                        assert(0);
                }
            }

            SceneObjectRotation(Vector3f& axis, const float theta)
            {
                Normalize(axis);
                MatrixRotationAxis(m_matrix, axis, theta);
            }

            SceneObjectRotation(const Quaternion quaternion)
            {
                MatrixRotationQuaternion(m_matrix, quaternion);
            }
    };

    class SceneObjectScale : public SceneObjectTransform
    {
        public:
            SceneObjectScale(const char axis, const float amount)  
            { 
                switch (axis) {
                    case 'x':
                        MatrixScale(m_matrix, amount, 0.0f, 0.0f);
                        break;
                    case 'y':
                        MatrixScale(m_matrix, 0.0f, amount, 0.0f);
                        break;
                    case 'z':
                        MatrixScale(m_matrix, 0.0f, 0.0f, amount);
                        break;
                    default:
                        assert(0);
                }
            }

            SceneObjectScale(const float x, const float y, const float z) 
            {
                MatrixScale(m_matrix, x, y, z);
            }
    };

}