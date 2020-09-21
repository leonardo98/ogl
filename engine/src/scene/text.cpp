#include "text.h"

#include "system/paths.h"

#include "rapidxml/rapidxml.hpp"
#include <glm/gtc/matrix_transform.inl>

#include <fstream>

using namespace tst;

bool Parse(std::shared_ptr<rapidxml::xml_document<>> doc, std::vector<char>& buffer, const char* fileName)
{
    try
    {
        doc->parse<0>(buffer.data());
        return true;
    }
    catch (const std::runtime_error& e)
    {
        printf("%s Runtime error was: %s", fileName, e.what());
    }
    catch (const rapidxml::parse_error& e)
    {
        printf("%s Parse error was: %s", fileName, e.what());
    }
    catch (const std::exception& e)
    {
        printf("%s Error was: %s", fileName, e.what());
    }
    catch (...)
    {
        printf("%s An unknown error occurred.", fileName);
    }
    return false;
}

Text::Text(const char *fileName)
    : _text("")
    , _baseLineHeight(0.f)
    , _lineHeight(0.f)
    , _textWidth(0.f)
    , _textHeight(0.f)
    , _horAlign(HorAlign::Left)
    , _vertAlign(VertAlign::Top)
{
    std::string fileNameFullPath = GetPath(fileName);

    std::ifstream myfile(fileNameFullPath);
    std::vector<char> buffer;
    buffer = std::vector<char>(std::istreambuf_iterator<char>(myfile), std::istreambuf_iterator<char>());
    if (buffer.size() == 0)
    {
        assert(false);
        //LOG("Error! Font not found! " + filename);
        return;
    }

    buffer.push_back('\0');
    std::shared_ptr<rapidxml::xml_document<>> doc = std::make_shared<rapidxml::xml_document<>>();
    if (!Parse(doc, buffer, fileName))
    {
        assert(false);
        return;
    }

    // Parse font description
    rapidxml::xml_node<>* root = doc->first_node();
    assert(root);
    if (root)
    {
        float scaleW;
        float scaleH;

        rapidxml::xml_node<>* common = root->first_node("common");
        assert(common);
        if (common)
        {
            _lineHeight = atof(common->first_attribute("lineHeight")->value());
            _baseLineHeight = atof(common->first_attribute("base")->value());
            scaleW = atof(common->first_attribute("scaleW")->value());
            scaleH = atof(common->first_attribute("scaleH")->value());
        }

        rapidxml::xml_node<>* chars = root->first_node("chars");
        assert(chars);
        if (chars)
        {
            _letters.resize(256);
            rapidxml::xml_node<>* symbol = chars->first_node("char");
            while (symbol)
            {
                unsigned int code = (unsigned int)atoi(symbol->first_attribute("id")->value());
                unsigned char charCode = static_cast<unsigned char>(code);
                assert(code == charCode);

                //unsigned int page = (unsigned int)atoi(symbol->first_attribute("page")->value());

                int x = atoi(symbol->first_attribute("x")->value());
                int y = atoi(symbol->first_attribute("y")->value());
                int w = atoi(symbol->first_attribute("width")->value());
                int h = atoi(symbol->first_attribute("height")->value());

                int xOffset = atoi(symbol->first_attribute("xoffset")->value());
                int yOffest = atoi(symbol->first_attribute("yoffset")->value());
                int xadvance = atoi(symbol->first_attribute("xadvance")->value());

                _letters[code] = MakeLetter(x, y, w, h, scaleW, scaleH, xOffset, yOffest, xadvance);

                symbol = symbol->next_sibling("char");
            }
        }

        rapidxml::xml_node<>* kernings = root->first_node("kernings");
        assert(kernings);
        if (kernings)
        {
            _kernings.resize(256, std::vector<char>(255, 0));
            rapidxml::xml_node<>* kerning = kernings->first_node("kerning");
            while (kerning)
            {
                unsigned int first = (unsigned int)atoi(kerning->first_attribute("first")->value());
                unsigned int second = (unsigned int)atoi(kerning->first_attribute("second")->value());

                {
                    unsigned char charCode = static_cast<unsigned char>(first);
                    assert(first == charCode);
                    assert(_letters[first]);
                    
                    charCode = static_cast<unsigned char>(second);
                    assert(second == charCode);
                    assert(_letters[second]);
                }

                int amount = atoi(kerning->first_attribute("amount")->value());

                _kernings[first][second] = amount;

                kerning = kerning->next_sibling("kerning");
            }
        }
    }
}

void Text::Render(const RenderState& rs) const
{
    RenderState result{ rs.matrix * _local, rs.alpha * GetAlpha(), rs.batch };
    assert(rs.batch);
    if (rs.batch)
    {
        glm::vec3 shift(0.f);
        std::string value = _text.GetValueLF();
        if (value.empty())
        {
            return;
        }

        unsigned int lineCount = 0;

        for (std::string::size_type line = 0; line < _lineStarts.size(); ++line)
        {
            std::string::size_type s = _lineStarts[line];
            std::string::size_type e = (line + 1) < _lineStarts.size() ? _lineStarts[line + 1] : value.size();

            RenderState renderLetter{ result.matrix, result.alpha, rs.batch };
            shift.x = 0.f;
            shift.y = _lineHeight * line;

            if (_horAlign == HorAlign::Center)
            {
                shift.x -= (_lineWidths[line] / 2);
            }
            else if (_horAlign == HorAlign::Right)
            {
                shift.x -= _lineWidths[line];
            }

            if (_vertAlign == VertAlign::Center)
            {
                shift.y -= _textHeight / 2;
            }
            else if (_vertAlign == VertAlign::Bottom)
            {
                shift.y -= _textHeight;
            }

            for (std::string::size_type i = s; i < e; i++)
            {
                unsigned char c = value[i];
                assert(c <= 255);
                auto letter = _letters[value[i]];
                if (letter)
                {
                    renderLetter.matrix = glm::translate(renderLetter.matrix, shift);
                    rs.batch->AddQuad(letter->quad, renderLetter);

                    shift.x = letter->xadvance;
                    shift.y = 0;
                    if (i + 1 < value.size() && value[i + 1] != '\n')
                    {
                        shift.x += _kernings[value[i]][value[i + 1]];
                    }
                }
            }

        }

    }
    Actor::RenderChild(result);
}

void Text::SetText(const std::string &text)
{
    _text.SetValue(text);

    _lineStarts.clear();
    _lineWidths.clear();
    _textWidth = 0.f;

    _lineStarts.push_back(0);
    float w = 0.f;
    for (std::string::size_type i = 0; i < text.size(); i++)
    {
        if (text[i] != '\n')
        {
            auto letter = _letters[text[i]];
            if (letter)
            {
                w += letter->xadvance;
                if (i + 1 < text.size() && text[i + 1] != '\n')
                {
                    w += _kernings[text[i]][text[i + 1]];
                }
            }
        }
        else
        {
            if (_textWidth < w)
            {
                _textWidth = w;
            }
            _lineWidths.push_back(w);
            w = 0.f;

            _lineStarts.push_back(i + 1);
        }
    }
    _lineWidths.push_back(w);

    if (_lineStarts.size() > 0)
    {
        _textHeight = (_lineStarts.size() - 1) * _lineHeight + _baseLineHeight;
    }
    else
    {
        _textHeight = 0.f;
    }
}

void Text::SetAlign(HorAlign horAlign, VertAlign verAlign)
{
    _horAlign = horAlign;
    _vertAlign = verAlign;
}

Vertex Text::CalcVertex(int x, int y, int tx, int ty, float scaleW, float scaleH) const
{
    Vertex v;
    v.x = x;
    v.y = y;
    v.z = 0.f;
    v.u = tx / scaleW;
    v.v = ty / scaleW;
    v.alpha = 1.f;
    return v;
}

std::shared_ptr<Letter> Text::MakeLetter(int x, int y, int width, int height, float scaleW, float scaleH, int xoffset, int yoffset, int xadvance) const
{
    std::shared_ptr<Letter> letter = std::make_shared<Letter>();
    letter->xadvance = xadvance;
    letter->quad.vertices[0] = CalcVertex(xoffset,         yoffset,                  x, y,          scaleW, scaleH);
    letter->quad.vertices[1] = CalcVertex(xoffset + width, yoffset,          x + width, y,          scaleW, scaleH);
    letter->quad.vertices[2] = CalcVertex(xoffset + width, yoffset + height, x + width, y + height, scaleW, scaleH);
    letter->quad.vertices[3] = CalcVertex(xoffset,         yoffset + height,         x, y + height, scaleW, scaleH);
    return letter;
}
