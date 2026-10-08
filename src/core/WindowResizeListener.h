#ifndef WIREBOUNDWORLDCREATOR_WINDOWRESIZELISTENER_H
#define WIREBOUNDWORLDCREATOR_WINDOWRESIZELISTENER_H

class WindowResizeListener {
public:
  virtual ~WindowResizeListener() = default;
  virtual void OnWindowResize() {}
};

#endif  // WIREBOUNDWORLDCREATOR_WINDOWRESIZELISTENER_H
