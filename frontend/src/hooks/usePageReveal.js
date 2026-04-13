import { useEffect } from "react";
import gsap from "gsap";

export function usePageReveal(selector = ".reveal") {
  useEffect(() => {
    const items = gsap.utils.toArray(selector);
    gsap.fromTo(
      items,
      { opacity: 0, y: 30 },
      { opacity: 1, y: 0, duration: 0.7, stagger: 0.08, ease: "power3.out" }
    );
  }, [selector]);
}
