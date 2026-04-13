import { useEffect } from "react";
import { useLocation } from "react-router-dom";
import gsap from "gsap";

const MOBILE_BREAKPOINT = 768;
const PARTICLE_COUNT = 10;
const SPOTLIGHT_RADIUS = 280;

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function distanceToRect(x, y, rect) {
  const dx = Math.max(rect.left - x, 0, x - rect.right);
  const dy = Math.max(rect.top - y, 0, y - rect.bottom);
  return Math.hypot(dx, dy);
}

function createParticle(x, y) {
  const particle = document.createElement("span");
  particle.className = "magic-card-particle";
  particle.style.left = `${x}px`;
  particle.style.top = `${y}px`;
  return particle;
}

function createRipple(card, x, y) {
  const maxDistance = Math.max(
    Math.hypot(x, y),
    Math.hypot(x - card.clientWidth, y),
    Math.hypot(x, y - card.clientHeight),
    Math.hypot(x - card.clientWidth, y - card.clientHeight)
  );

  const ripple = document.createElement("span");
  ripple.className = "magic-card-ripple";
  ripple.style.width = `${maxDistance * 2}px`;
  ripple.style.height = `${maxDistance * 2}px`;
  ripple.style.left = `${x - maxDistance}px`;
  ripple.style.top = `${y - maxDistance}px`;
  return ripple;
}

export default function MagicCardEffects() {
  const location = useLocation();

  useEffect(() => {
    if (typeof window === "undefined") {
      return undefined;
    }

    const isCoarsePointer = window.matchMedia("(pointer: coarse)").matches;
    const isMobile = window.innerWidth <= MOBILE_BREAKPOINT || isCoarsePointer;

    if (isMobile) {
      return undefined;
    }

    const spotlight = document.createElement("div");
    spotlight.className = "global-card-spotlight";
    document.body.appendChild(spotlight);

    const particleRegistry = new WeakMap();
    const cardTeardownMap = new Map();

    const clearParticles = (card) => {
      const registry = particleRegistry.get(card);
      if (!registry) {
        return;
      }

      registry.timeouts.forEach((id) => window.clearTimeout(id));
      registry.timeouts = [];

      registry.particles.forEach((particle) => {
        gsap.to(particle, {
          scale: 0,
          opacity: 0,
          duration: 0.2,
          ease: "power2.out",
          onComplete: () => {
            particle.remove();
          },
        });
      });

      registry.particles = [];
    };

    const attachCard = (card) => {
      if (!(card instanceof HTMLElement) || cardTeardownMap.has(card)) {
        return;
      }

      card.classList.add("magic-card-active");
      card.style.setProperty("--magic-x", "50%");
      card.style.setProperty("--magic-y", "50%");
      card.style.setProperty("--magic-intensity", "0");

      gsap.set(card, {
        transformPerspective: 1000,
        transformStyle: "preserve-3d",
      });

      const handlePointerEnter = () => {
        spawnParticles(card);
        gsap.to(card, {
          scale: 1.01,
          duration: 0.22,
          ease: "power2.out",
        });
      };

      const handlePointerMove = (event) => {
        const rect = card.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        const rx = ((y - rect.height / 2) / rect.height) * -8;
        const ry = ((x - rect.width / 2) / rect.width) * 8;

        const tx = ((x - rect.width / 2) / rect.width) * 8;
        const ty = ((y - rect.height / 2) / rect.height) * 6;

        card.style.setProperty("--magic-x", `${(x / rect.width) * 100}%`);
        card.style.setProperty("--magic-y", `${(y / rect.height) * 100}%`);
        card.style.setProperty("--magic-intensity", "0.85");

        gsap.to(card, {
          rotateX: rx,
          rotateY: ry,
          x: tx,
          y: ty,
          duration: 0.14,
          ease: "power2.out",
          overwrite: true,
        });
      };

      const handlePointerLeave = () => {
        clearParticles(card);
        card.style.setProperty("--magic-intensity", "0");

        gsap.to(card, {
          rotateX: 0,
          rotateY: 0,
          x: 0,
          y: 0,
          scale: 1,
          duration: 0.26,
          ease: "power2.out",
        });
      };

      const handleClick = (event) => {
        const rect = card.getBoundingClientRect();
        const ripple = createRipple(card, event.clientX - rect.left, event.clientY - rect.top);

        card.appendChild(ripple);

        gsap.fromTo(
          ripple,
          { scale: 0, opacity: 1 },
          {
            scale: 1,
            opacity: 0,
            duration: 0.7,
            ease: "power2.out",
            onComplete: () => ripple.remove(),
          }
        );
      };

      card.addEventListener("pointerenter", handlePointerEnter);
      card.addEventListener("pointermove", handlePointerMove);
      card.addEventListener("pointerleave", handlePointerLeave);
      card.addEventListener("click", handleClick);

      cardTeardownMap.set(card, () => {
        clearParticles(card);
        card.removeEventListener("pointerenter", handlePointerEnter);
        card.removeEventListener("pointermove", handlePointerMove);
        card.removeEventListener("pointerleave", handlePointerLeave);
        card.removeEventListener("click", handleClick);
      });
    };

    const detachMissingCards = (nextCards) => {
      const nextSet = new Set(nextCards);
      Array.from(cardTeardownMap.keys()).forEach((existingCard) => {
        if (!nextSet.has(existingCard)) {
          const teardown = cardTeardownMap.get(existingCard);
          teardown?.();
          cardTeardownMap.delete(existingCard);
        }
      });
    };

    const scanCards = () => {
      const foundCards = Array.from(
        new Set([
          ...Array.from(document.querySelectorAll(".ink-card")),
          ...Array.from(document.querySelectorAll("[data-magic-card]")),
        ])
      ).filter((card) => card instanceof HTMLElement);

      foundCards.forEach((card) => attachCard(card));
      detachMissingCards(foundCards);
    };

    const spawnParticles = (card) => {
      clearParticles(card);

      const rect = card.getBoundingClientRect();
      const timeouts = [];
      const particles = [];

      for (let i = 0; i < PARTICLE_COUNT; i += 1) {
        const timeoutId = window.setTimeout(() => {
          if (!card.matches(":hover")) {
            return;
          }

          const particle = createParticle(
            Math.random() * rect.width,
            Math.random() * rect.height
          );

          card.appendChild(particle);
          particles.push(particle);

          gsap.fromTo(
            particle,
            { scale: 0, opacity: 0 },
            { scale: 1, opacity: 0.7, duration: 0.22, ease: "back.out(2)" }
          );

          gsap.to(particle, {
            x: (Math.random() - 0.5) * 90,
            y: (Math.random() - 0.5) * 70,
            duration: 1.8 + Math.random() * 1.4,
            ease: "sine.inOut",
            repeat: -1,
            yoyo: true,
          });

          gsap.to(particle, {
            opacity: 0.2,
            duration: 1.2,
            ease: "power1.inOut",
            repeat: -1,
            yoyo: true,
          });
        }, i * 80);

        timeouts.push(timeoutId);
      }

      particleRegistry.set(card, { particles, timeouts });
    };

    scanCards();

    let rafId = null;
    const observer = new MutationObserver(() => {
      if (rafId) {
        return;
      }
      rafId = window.requestAnimationFrame(() => {
        scanCards();
        rafId = null;
      });
    });

    observer.observe(document.body, {
      childList: true,
      subtree: true,
    });

    const handleDocumentMove = (event) => {
      const cards = Array.from(cardTeardownMap.keys());
      if (cards.length === 0) {
        return;
      }

      let nearest = Infinity;

      cards.forEach((card) => {
        const rect = card.getBoundingClientRect();
        const distance = distanceToRect(event.clientX, event.clientY, rect);

        nearest = Math.min(nearest, distance);

        const relativeX = clamp((event.clientX - rect.left) / rect.width, 0, 1);
        const relativeY = clamp((event.clientY - rect.top) / rect.height, 0, 1);

        card.style.setProperty("--magic-x", `${relativeX * 100}%`);
        card.style.setProperty("--magic-y", `${relativeY * 100}%`);

        const intensity = clamp(1 - distance / SPOTLIGHT_RADIUS, 0, 1);
        if (!card.matches(":hover")) {
          card.style.setProperty("--magic-intensity", intensity.toFixed(3));
        }
      });

      gsap.to(spotlight, {
        left: event.clientX,
        top: event.clientY,
        duration: 0.12,
        ease: "power2.out",
        overwrite: true,
      });

      const opacity = clamp(1 - nearest / (SPOTLIGHT_RADIUS * 1.1), 0, 1) * 0.7;

      gsap.to(spotlight, {
        opacity,
        duration: opacity > 0 ? 0.2 : 0.35,
        ease: "power2.out",
        overwrite: true,
      });
    };

    const handleDocumentLeave = () => {
      gsap.to(spotlight, {
        opacity: 0,
        duration: 0.35,
        ease: "power2.out",
      });

      Array.from(cardTeardownMap.keys()).forEach((card) => {
        if (!card.matches(":hover")) {
          card.style.setProperty("--magic-intensity", "0");
        }
      });
    };

    document.addEventListener("pointermove", handleDocumentMove);
    document.addEventListener("pointerleave", handleDocumentLeave);

    return () => {
      document.removeEventListener("pointermove", handleDocumentMove);
      document.removeEventListener("pointerleave", handleDocumentLeave);

      if (rafId) {
        window.cancelAnimationFrame(rafId);
      }

      observer.disconnect();
      Array.from(cardTeardownMap.values()).forEach((teardown) => teardown());
      cardTeardownMap.clear();
      spotlight.remove();
    };
  }, [location.pathname]);

  return null;
}
