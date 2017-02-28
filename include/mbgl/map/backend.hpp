#pragma once

#include <mbgl/map/change.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/size.hpp>

#include <memory>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class BackendScope;

class Backend {
public:
    Backend();
    virtual ~Backend();

    // Returns the backend's context which manages OpenGL state.
    gl::Context& getContext();

    using glProc = void (*)();
    void initializeExtensions(glProc (*getProcAddress)(const char*));

    // Called when the map needs to be rendered; the backend should call Map::render() at some point
    // in the near future. (Not called for Map::renderStill() mode.)
    virtual void invalidate() = 0;

    // Notifies a watcher of map x/y/scale/rotation changes.
    virtual void notifyMapChange(MapChange change);

    enum class UpdateType : bool {
        Silent = false,
        Force = true,
    };

    PremultipliedImage readFramebuffer(const Size&) const;
    void invalidateFramebufferBinding();
    void updateFramebufferBinding(uint32_t fbo, UpdateType = UpdateType::Silent);
    uint32_t getFramebufferBinding();
    void updateViewportSize(const Size&, UpdateType = UpdateType::Silent);

protected:
    // Called when the backend's GL context needs to be made active or inactive. These are called,
    // as a matched pair, in four situations:
    //
    //   1. When releasing GL resources during Map destruction
    //   2. When calling a CustomLayerInitializeFunction, during Map::addLayer
    //   3. When calling a CustomLayerDeinitializeFunction, during Map::removeLayer
    //   4. When rendering for Map::renderStill
    //
    // They are *not* called for Map::render; it is assumed that the correct context is already
    // activated prior to calling Map::render.
    virtual void activate() = 0;
    virtual void deactivate() = 0;

    std::unique_ptr<gl::Context> context;
    bool extensionsInitialized = false;

    friend class BackendScope;
};

} // namespace mbgl
