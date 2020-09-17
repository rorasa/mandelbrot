
name := "create-mandelbrot"

version := "0.1"

scalaVersion := "2.13.3"

assemblyMergeStrategy in assembly := {

  case PathList("META-INF", xs @ _*) => MergeStrategy.discard
  case x => MergeStrategy.first
}
