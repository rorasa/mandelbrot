package com.rorasa.mandelbrot

import java.awt.image.BufferedImage
import java.awt.Graphics2D
import java.io.File
import javax.imageio.ImageIO

import scala.math.sqrt


object MandelbrotCreator {

  def TestMandelbrotMembership(c: ComplexNumber, limit: Int): Boolean = {
    var z = ComplexNumber(0,0)
    for (_ <- 0 until limit){
      z = z.multiply(z).add(c)
      if (z.magnitude() > 2.0){
        return false
      }
    }
    return true
  }

  def CreateMandelbrotSet(image_size: Int): BinaryMatrix ={

    val low_limit_re = -0.5
    val high_limit_re = 0
    val low_limit_im = -1
    val high_limit_im = -0.5

    val mandelbrot_set = new BinaryMatrix()
    mandelbrot_set.init(image_size)

    val re_resolution = image_size.toDouble/(high_limit_re - low_limit_re)
    val im_resolution = image_size.toDouble/(high_limit_im - low_limit_im)

    for(i <- 0 until image_size){
      for(j <- 0 until image_size){
        val re = ( i / re_resolution) + low_limit_re
        val im = ( j / im_resolution) + low_limit_im

        if(TestMandelbrotMembership( ComplexNumber(re,im), 1000 )){
          mandelbrot_set.setValue(j,i)
        }
      }

      if(i%10 == 0){
        println("Processing row "+i.toString)
      }
    }

    mandelbrot_set
  }

  def main(args: Array[String]): Unit = {
    if(args.length==0){
      throw new IllegalArgumentException("Missing image size")
    }

    val image_size: Int = args(0).toInt

    if ((image_size % 4) != 0) {
      println("Warning, image_size should be dividable by 4")
    }

    val mandelbrot_set = CreateMandelbrotSet(image_size)
    mandelbrot_set.toBitmap()
  }
}

case class ComplexNumber(re: Double, im: Double){
  def multiply(c: ComplexNumber): ComplexNumber ={
    val re = (this.re * c.re) - (this.im * c.im)
    val im = (this.re * c.im) + (this.im * c.re)
    ComplexNumber(re, im)
  }

  def add(c: ComplexNumber): ComplexNumber = {
    val re = this.re + c.re
    val im = this.im + c.im
    ComplexNumber(re, im)
  }

  def magnitude(): Double = {
    sqrt((this.re.toDouble * this.re.toDouble) + (this.im.toDouble * this.im.toDouble))
  }
}

class BinaryMatrix(){
  var data: Seq[Seq[Boolean]] = Seq()
  var image_size = 0

  def init(size: Int): Unit = {
    this.image_size = size

    var mat: Seq[Seq[Boolean]] = Seq()
    for(i<-0 until size){
      var row: Seq[Boolean] = Seq()
      for(_<-0 until size){
        row = row :+ false
      }
      mat = mat :+ row
    }

    this.data = mat
  }

  def setValue(row: Int, col: Int): Unit = {
    val oldRow = this.data(row)
    val newRow = oldRow.updated(col, true)
    this.data = this.data.updated(row, newRow)
  }

  def resetValue(row: Int, col: Int): Unit = {
    val oldRow = this.data(row)
    val newRow = oldRow.updated(col, false)
    this.data = this.data.updated(row, newRow)
  }

  def toBitmap(): Unit = {
    var image = new BufferedImage(this.image_size, this.image_size, BufferedImage.TYPE_INT_ARGB)
    val grahics: Graphics2D = image.createGraphics

    val white = 0xFF << 24 | 0xFF << 16 | 0xFF << 8 | 0xFF
    val black = 0xFF << 24 | 0x00 << 16 | 0x00 << 8 | 0x00

    for(i<-0 until this.image_size){
      for(j<-0 until this.image_size){
        if(this.data(i)(j)){
          image.setRGB(j,i,white)
        }else{
          image.setRGB(j,i,black)
        }
      }
    }

    val outputfile: File = new File("output.png")
    ImageIO.write(image, "png", outputfile)
  }

}
