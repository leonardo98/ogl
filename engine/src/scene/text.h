#pragma once

#ifndef TST_TEXT_H
#define TST_TEXT_H

#include "core/quad.h"

#include "actor.h"
#include "texture.h"

#include <vector>

namespace tst
{
    enum class HorAlign
    {
        Left,
        Center,
        Right,
    };

    enum class VertAlign
    {
        Top,
        Center,
        Bottom,
    };

    struct Letter
    {
        int xadvance;
        Quad quad;
    };

    class Text : public Actor
    {
    public:
        // методы вызываются из игрового потока
        Text(const char *fileName);

        void SetText(const std::string &text);
        void SetAlign(HorAlign horAlign, VertAlign verAlign);

    protected:
        // методы вызываются из главного потока
        virtual void Render(const RenderState& rs) const override;
        
    private:
        Vertex CalcVertex(int x, int y, int tx, int ty, float scaleW, float scaleH) const;
        std::shared_ptr<Letter> MakeLetter(int x, int y, int w, int height, float scaleW, float scaleH, int xoffset, int yoffset, int xadvance) const;

        // к полям вызываются из любого потока, но не конкурентно, только на чтение, mutex не нужен
        
        // fonts attributes
        std::vector<std::shared_ptr<Letter>> _letters;
        float _baseLineHeight; // высота строки (однострочный вариант)
        float _lineHeight;     // высота/смещение строки при многострочном тексте
        std::vector<std::vector<char>> _kernings;

        // text attributes
        std::vector<float> _lineWidths;
        std::vector<int> _lineStarts;
        float _textWidth;
        float _textHeight;

        // user properties
        GameVar<std::string> _text;
        std::atomic<HorAlign> _horAlign;
        std::atomic<VertAlign> _vertAlign;
    };
}

#endif//TST_TEXT_H
